// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Interfaces/InteractableInterface.h"
#include "LootableSupplies.generated.h"

UCLASS()
class ADVENTUREOPENWORLD_API ALootableSupplies : public AItem, public IInteractableInterface
{
	GENERATED_BODY()
	
public:	
	ALootableSupplies();

	virtual void Tick(float DeltaTime) override;

	/*  <IInteractableInterface> */
	virtual void SetIsBeignActivated(bool IsInteracting) override;
	virtual void SetBarPercent(const float& Percent, float DeltaTime) override;
	virtual void OnInteractionConcluded() override;
	/* /<IInteractableInterface> */

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Interactable Properties")
	float LootingTime;

private:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> SecondaryMesh;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class URadialBarComponent> RadialBarComponent;

public:	
	

};
