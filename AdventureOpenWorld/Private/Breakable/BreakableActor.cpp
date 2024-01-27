#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/BoxComponent.h"
#include "Items/Pickups/Treasure.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>("Geometry Collection Component");
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	SetRootComponent(GeometryCollection);

	VolumeBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Volume Box"));
	VolumeBox->SetupAttachment(GetRootComponent());
	VolumeBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	VolumeBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint)
{
	FVector SpawnLocation = GetActorLocation();
	SpawnLocation.Z += 75.f;

	if (GetWorld() && TreasureClass)
		GetWorld()->SpawnActor<ATreasure>(TreasureClass, SpawnLocation, GetActorRotation());

}
