// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class ADVENTUREOPENWORLD_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttributeComponent();

	void RegenStamina(float DeltaTime);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaDodgeCost;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float WalkSpeed;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float RunSpeed;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Gold;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Souls;

public:
	void ReceiveDamage(float Damage);
	void Heal(float HealAmmount);
	void UseStamina(float StaminaCost);
	float GetHealthPercent();
	float GetStaminaPercent();
	bool IsAlive() const;
	bool IsHurt() const;
	void AddGold(int32 AmmountToAdd);
	void AddSouls(int32 AmmountToAdd);

	FORCEINLINE float GetWalkSpeed() const { return WalkSpeed; }
	FORCEINLINE float GetRunSpeed() const { return RunSpeed; }
	FORCEINLINE int32 GetGold() const { return Gold; }
	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE float GetMaxStamina() const { return MaxStamina; }
	FORCEINLINE float GetStaminaDodgeCost() const { return StaminaDodgeCost; }
};
