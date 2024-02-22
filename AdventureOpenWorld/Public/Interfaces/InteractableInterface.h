// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ADVENTUREOPENWORLD_API IInteractableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
protected:
	/** Pickup Item Variables */
	bool bIsBeignActivated = false;
	bool bInteractionConcluded = false;

	float InteractionFill = 0.f;
	/** Time necessary to hold pickup key for item*/
	float InteractionTime = 1.5f;

public:
	virtual void SetIsBeignActivated(bool IsInteracting);
	virtual void SetBarPercent(const float& Percent, float DeltaTime);
	virtual void OnInteractionConcluded();

	FORCEINLINE bool GetInteractionConcluded() const { return bInteractionConcluded; }
};
