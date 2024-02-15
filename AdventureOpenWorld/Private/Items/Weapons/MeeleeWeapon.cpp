#include "Items/Weapons/MeeleeWeapon.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/HitInterface.h"

AMeeleeWeapon::AMeeleeWeapon()
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

void AMeeleeWeapon::Unequip()
{
	Super::Unequip();

	ItemMesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
}

void AMeeleeWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	ItemMesh->AttachToComponent(
		InParent,
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		InSocketName
	);
}

void AMeeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AMeeleeWeapon::OnBoxOverlap);
}

void AMeeleeWeapon::ClearIgnoreActors()
{
	IgnoreActors.Empty();
}

void AMeeleeWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (HitActorIsSameType(OtherActor) || !Cast<IHitInterface>(OtherActor)) return;

	FHitResult OutBoxHit;
	BoxTrace(OutBoxHit);

	if (OutBoxHit.GetActor())
	{
		if (HitActorIsSameType(OutBoxHit.GetActor())) return;

		UGameplayStatics::ApplyDamage(OutBoxHit.GetActor(), Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());
		ExecuteGetHit(OutBoxHit);
		//CreateFields(OutBoxHit.ImpactPoint);
	}
}

void AMeeleeWeapon::ExecuteGetHit(FHitResult& OutBoxHit)
{
	if (IHitInterface* HitInterface = Cast<IHitInterface>(OutBoxHit.GetActor()))
		HitInterface->Execute_GetHit(OutBoxHit.GetActor(), OutBoxHit.ImpactPoint, GetOwner());

}

bool AMeeleeWeapon::HitActorIsSameType(AActor* OtherActor)
{
	return GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"));
}

void AMeeleeWeapon::BoxTrace(FHitResult& OutBoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());
	for (AActor* Actor : IgnoreActors) ActorsToIgnore.AddUnique(Actor);

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
