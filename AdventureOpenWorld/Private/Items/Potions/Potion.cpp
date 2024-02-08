#include "Items/Potions/Potion.h"
#include "Interfaces/PickupInterface.h"

void APotion::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor))
	{
		if (PickupInterface->CollectHealthPotion())
		{
			SpawnPickupEffect();
			SpawnPickupSound();
			Destroy();
		}
	}
}
