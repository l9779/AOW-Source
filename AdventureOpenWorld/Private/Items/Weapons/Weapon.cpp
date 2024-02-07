#include "Items/Weapons/Weapon.h"
#include "Characters/SlashCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Interfaces/HitInterface.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon(): 
	Damage(20.f)
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator, bool bPlaySound)
{
	ItemState = EItemState::EIS_Equipped;
	AttachMeshToSocket(InParent, InSocketName);
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	DisableSphereCollision();

	if (bPlaySound) PlayEquipSound();
	DeactivateEmbers();
}

void AWeapon::DeactivateEmbers()
{
	if (ItemEffect) ItemEffect->Deactivate();
}

void AWeapon::DisableSphereCollision()
{
	if (Sphere) Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound)
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	ItemMesh->AttachToComponent(
		InParent, 
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		InSocketName
	);
}

void AWeapon::Unequip()
{
	SetOwner(nullptr);
	SetInstigator(nullptr);

	ItemMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	ItemState = EItemState::EIS_Hovering;

	if (Sphere) Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HitActorIsSameType(OtherActor)) return;
	
	FHitResult OutBoxHit;
	BoxTrace(OutBoxHit);

	if (OutBoxHit.GetActor()) 
	{
		if (HitActorIsSameType(OutBoxHit.GetActor())) return;

		UGameplayStatics::ApplyDamage(OutBoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		ExecuteGetHit(OutBoxHit);
		CreateFields(OutBoxHit.ImpactPoint);
	}
}

bool AWeapon::HitActorIsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

void AWeapon::ExecuteGetHit(FHitResult& OutBoxHit)
{
	if (IHitInterface* HitInterface = Cast<IHitInterface>(OutBoxHit.GetActor()))
		HitInterface->Execute_GetHit(OutBoxHit.GetActor(), OutBoxHit.ImpactPoint, GetOwner());
}

void AWeapon::BoxTrace(FHitResult& OutBoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.AddUnique(GetOwner());
	for (AActor* Actor : IgnoreActors) ActorsToIgnore.Add(Actor);

	UKismetSystemLibrary::BoxTraceSingle(
 		this,
		Start,
		End,
		BoxTraceExtent,
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		OutBoxHit,
		true
	);

	ActorsToIgnore.AddUnique(OutBoxHit.GetActor());
}
