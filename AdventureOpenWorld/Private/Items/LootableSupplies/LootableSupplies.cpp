#include "Items/LootableSupplies/LootableSupplies.h"
#include "Components/BoxComponent.h"
#include "HUD/RadialBarComponent.h"

ALootableSupplies::ALootableSupplies():
	LootingTime(2.f)
{
	PrimaryActorTick.bCanEverTick = true;

	SecondaryMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Seconday Mesh"));
	SecondaryMesh->SetupAttachment(ItemMesh);
	SecondaryMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	RadialBarComponent = CreateDefaultSubobject<URadialBarComponent>(TEXT("Radial Bar Component"));
	RadialBarComponent->SetupAttachment(GetRootComponent());
	RadialBarComponent->SetVisibility(false);
	RadialBarComponent->SetDrawSize(FVector2D(80.f, 80.f));
}

void ALootableSupplies::BeginPlay()
{
	Super::BeginPlay();
	
	InteractionTime = LootingTime;

}

void ALootableSupplies::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsBeignActivated) 
		SetBarPercent(InteractionFill / InteractionTime, DeltaTime);
}

void ALootableSupplies::SetIsBeignActivated(bool IsInteracting)
{
	if (bInteractionConcluded) return;

	IInteractableInterface::SetIsBeignActivated(IsInteracting);

	if (RadialBarComponent) RadialBarComponent->SetVisibility(IsInteracting);
}

void ALootableSupplies::SetBarPercent(const float& Percent, float DeltaTime)
{
	IInteractableInterface::SetBarPercent(Percent, DeltaTime);

	if (RadialBarComponent) RadialBarComponent->SetBarPercent(Percent);
}

void ALootableSupplies::OnInteractionConcluded()
{
	SecondaryMesh->SetVisibility(false);
	if (RadialBarComponent) RadialBarComponent->SetVisibility(false);
}



