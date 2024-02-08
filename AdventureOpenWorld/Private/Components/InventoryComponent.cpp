#include "Components/InventoryComponent.h"

UInventoryComponent::UInventoryComponent():
	HealthPotionCount(0), HealthPotionLimit(100), HealthPotionRecovery(15.f)
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UInventoryComponent::UseHealthPotion()
{
	HealthPotionCount -= 1;
}

void UInventoryComponent::AddHealthPotion()
{
	HealthPotionCount += 1;
}

bool UInventoryComponent::HasSpaceForHealthPotion() const
{
	return HealthPotionCount < HealthPotionLimit;
}
