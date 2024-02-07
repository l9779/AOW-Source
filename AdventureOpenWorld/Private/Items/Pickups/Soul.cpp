#include "Items/Pickups/Soul.h"
#include "Interfaces/PickupInterface.h"


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
