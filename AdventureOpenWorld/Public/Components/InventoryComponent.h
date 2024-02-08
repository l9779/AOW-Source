// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVENTUREOPENWORLD_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	void UseHealthPotion();
	void AddHealthPotion();
	bool HasSpaceForHealthPotion() const;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Potions Properties")
	int32 HealthPotionCount;
	UPROPERTY(EditDefaultsOnly, Category = "Potions Properties")
	int32 HealthPotionLimit;
	UPROPERTY(EditAnywhere, Category = "Potions Properties")
	float HealthPotionRecovery;

public:
	int32 GetHealthPotionCount() const { return HealthPotionCount; }
	float GetHealthPotionRecovery() const { return HealthPotionRecovery; }
};
