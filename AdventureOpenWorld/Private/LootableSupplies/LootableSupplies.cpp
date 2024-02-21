#include "LootableSupplies/LootableSupplies.h"

ALootableSupplies::ALootableSupplies()
{
	PrimaryActorTick.bCanEverTick = true;

	MainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Main Mesh"));
	MainMesh->SetupAttachment(GetRootComponent());
	MainMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	SecondaryMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Seconday Mesh"));
	SecondaryMesh->SetupAttachment(MainMesh);
	SecondaryMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

// Called when the game starts or when spawned
void ALootableSupplies::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALootableSupplies::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

