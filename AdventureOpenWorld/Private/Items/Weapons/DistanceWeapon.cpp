#include "Items/Weapons/DistanceWeapon.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Projectiles/Projectile.h"

ADistanceWeapon::ADistanceWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	RootComponent = SkeletalMesh;

	ItemMesh->SetupAttachment(GetRootComponent());
	Sphere->SetupAttachment(GetRootComponent());
	ItemEffect->SetupAttachment(GetRootComponent());
}

void ADistanceWeapon::Unequip()
{
	Super::Unequip();

	SkeletalMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	SetArrowVisibility(true);
}

void ADistanceWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	SkeletalMesh->AttachToComponent(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		InSocketName
	);

	if (ItemState == EItemState::EIS_Holstered)
		SetArrowVisibility(false);
	else
		SetArrowVisibility(true);
}

void ADistanceWeapon::FireArrow()
{	
	APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(this, 0);
	FVector CrosshairWorldLocation = CameraManager->GetCameraLocation();
	FVector CrosshairImpactPoint = (CrosshairWorldLocation + (CameraManager->GetActorForwardVector() * 15000.f));
	
	FTransform ArrowSpawnTransform = GetArrowSpawnTransform(CrosshairWorldLocation, CrosshairImpactPoint);
	SpawnArrow(ArrowSpawnTransform);
}

void ADistanceWeapon::StringPulled(bool isPulled)
{
	SetArrowVisibility(isPulled);
	bStringPulled = isPulled;
}

void ADistanceWeapon::SpawnArrow(FTransform SpawnTransform)
{
	if (ArrowClass && GetWorld())
		if (AProjectile* Arrow = GetWorld()->SpawnActor<AProjectile>(ArrowClass, SpawnTransform))
		{
			Arrow->SetDamage(Damage);
			Arrow->SetOwner(this);
			Arrow->SetInstigator(GetInstigator());
		}
}

void ADistanceWeapon::SetArrowVisibility(bool Visible)
{
	if (Visible)
		SkeletalMesh->UnHideBoneByName(FName("arrow_nock"));
	else
		SkeletalMesh->HideBoneByName(FName("arrow_nock"), EPhysBodyOp::PBO_None);
}

FTransform ADistanceWeapon::GetArrowSpawnTransform(FVector& CrosshairWorldLocation, FVector& CrosshairImpactPoint) const
{
	FVector ImpactPoint = CrosshairImpactPoint;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	FHitResult OutHitResult;
	UKismetSystemLibrary::LineTraceSingle(
		this,
		CrosshairWorldLocation,
		ImpactPoint,
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHitResult,
		true
	);

	if (OutHitResult.bBlockingHit) ImpactPoint = OutHitResult.ImpactPoint;

	const FVector ArrowSpawnLocation = SkeletalMesh->GetSocketLocation(FName("ArrowSpawnSocket"));
	const FRotator ArrowSpawnRotation =
		UKismetMathLibrary::MakeRotFromX(ImpactPoint - ArrowSpawnLocation);

	return UKismetMathLibrary::MakeTransform(ArrowSpawnLocation, ArrowSpawnRotation);
}
