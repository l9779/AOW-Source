#include "Items/Pickups/Soul.h"
#include "Interfaces/PickupInterface.h"
#include "Components/SphereComponent.h"

void ASoul::BeginPlay()
{
	Super::BeginPlay();

	Sphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
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

void ASoul::EnableSphereCollision()
{
	Sphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
}
