// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RadialBar.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTUREOPENWORLD_API URadialBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetBarPercent(const float& Percent);

	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<class UImage> BarImage;
};
