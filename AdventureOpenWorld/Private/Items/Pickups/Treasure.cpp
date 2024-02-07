#include "Items/Pickups/Treasure.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/PickupInterface.h"

ATreasure::ATreasure()
{
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<IPickupInterface>(OtherActor))
	{
		SpawnPickupSound();
		Destroy();
	}
}
