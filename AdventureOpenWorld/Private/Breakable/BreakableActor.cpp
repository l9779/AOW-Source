#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/BoxComponent.h"
#include "Items/Pickups/Treasure.h"

ABreakableActor::ABreakableActor() :
	bBroken(false), bShouldSpawnTreasure(true)
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
	
	GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::OnChaosBreak);
}

void ABreakableActor::OnChaosBreak(const FChaosBreakEvent& BreakEvent)
{
	VolumeBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (bBroken) return;
	bBroken = true;

	if (GetWorld() && (TreasureClasses.Num() > 0) && bShouldSpawnTreasure)
	{
		FVector SpawnLocation = GetActorLocation();
		SpawnLocation.Z += 75.f;

		GetWorld()->SpawnActor<ATreasure>(
			TreasureClasses[FMath::RandRange(0, TreasureClasses.Num() - 1)],
			SpawnLocation, 
			GetActorRotation()
		);
	}

}
