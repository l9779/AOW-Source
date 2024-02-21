#include "Items/Pickups/Soul.h"
#include "Interfaces/PickupInterface.h"

void ASoul::BeginPlay()
{
	Super::BeginPlay();

	DesiredZ = GetDesiredZ();
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

	if (GetActorLocation().Z > DesiredZ)
		AddActorWorldOffset(FVector(0.f, 0.f, DriftRate * DeltaTime));
}
