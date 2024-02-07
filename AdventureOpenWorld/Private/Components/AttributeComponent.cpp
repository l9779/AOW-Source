#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent():
	Health(100.f), MaxHealth(100.f), 
	Stamina(100.f), MaxStamina(100.f), DodgeCost(25.f), StaminaRegenRate(2.f),
	WalkSpeed(125.f), RunSpeed(600.f)
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
}

void UAttributeComponent::UseStamina()
{
	Stamina = FMath::Clamp(Stamina - DodgeCost, 0.f, MaxStamina);
}

float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

float UAttributeComponent::GetStaminaPercent()
{
	return Stamina / MaxStamina;
}

bool UAttributeComponent::IsAlive()
{
	return Health > 0.f;
}

void UAttributeComponent::AddGold(int32 AmmountToAdd)
{
	Gold += AmmountToAdd;
}

void UAttributeComponent::AddSouls(int32 AmmountToAdd)
{
	Souls += AmmountToAdd;
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
}

