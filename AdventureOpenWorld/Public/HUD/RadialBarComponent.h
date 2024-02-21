// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "RadialBarComponent.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTUREOPENWORLD_API URadialBarComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	void SetBarPercent(const float& Percent);
private:
	UPROPERTY()
	TObjectPtr<class URadialBar> RadialBarWidget;
};
