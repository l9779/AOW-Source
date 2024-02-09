#include "Items/Pickups/Soul.h"
#include "Interfaces/PickupInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void ASoul::BeginPlay()
{
	Super::BeginPlay();

	const FVector Start = GetActorLocation();
	const FVector End = Start - FVector(0.f, 0.f, 2000.f);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	FHitResult OutHitResult;

	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start, End,
		ObjectTypes,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		OutHitResult,
		true);

	DesiredZ = OutHitResult.ImpactPoint.Z + 100.f;
}

void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor))
	{
		PickupInterface->AddSouls(this);
		SpawnPickupEffect();
		SpawnPickupSound();
		Destroy();
	}
	
}

void ASoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	const double LocationZ = GetActorLocation().Z;
	if (LocationZ > DesiredZ)
		AddActorWorldOffset(FVector(0.f, 0.f, DriftRate * DeltaTime));
}
