#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent():
	Health(100.f), MaxHealth(100.f), 
	Stamina(100.f), MaxStamina(100.f), StaminaDodgeCost(25.f), StaminaRegenRate(4.f),
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

void UAttributeComponent::Heal(float HealAmmount)
{
	Health = FMath::Clamp(Health + HealAmmount, 0.f, MaxHealth);
}

void UAttributeComponent::UseStamina(float StaminaCost)
{
	Stamina = FMath::Clamp(Stamina - StaminaCost, 0.f, MaxStamina);
}

float UAttributeComponent::GetHealthPercent()
{
	return Health / MaxHealth;
}

float UAttributeComponent::GetStaminaPercent()
{
	return Stamina / MaxStamina;
}

bool UAttributeComponent::IsAlive() const
{
	return Health > 0.f;
}

bool UAttributeComponent::IsHurt() const
{
	return Health < MaxHealth;
}

bool UAttributeComponent::NeedsToRecoverStamina() const
{
	return Stamina < MaxStamina;
}

void UAttributeComponent::AddGold(int32 AmmountToAdd)
{
	Gold += AmmountToAdd;
}

void UAttributeComponent::AddSouls(int32 AmmountToAdd)
{
	Souls += AmmountToAdd;
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
}

