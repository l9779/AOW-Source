// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SlashOverlay.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;

/**
 * Gameplay Overlay
 * WBP_SlashOverlay in-game
 */

UCLASS()
class ADVENTUREOPENWORLD_API USlashOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealthBarPercent(float Percent);
	void SetStaminaBarPercent(float Percent);
	void SetGold(int32 Gold);
	void SetSouls(int32 Souls);
	void SetPotionCount(int32 PotionAmmount);
	void SetCrosshairVisibility(ESlateVisibility SlateVisiblity);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaProgressBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GoldCountText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> SoulsCountText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PotionCountText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Crosshair;
};
