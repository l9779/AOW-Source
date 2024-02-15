#include "HUD/SlashOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void USlashOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar) HealthProgressBar->SetPercent(Percent);
}

void USlashOverlay::SetStaminaBarPercent(float Percent)
{
	if (StaminaProgressBar) StaminaProgressBar->SetPercent(Percent);
}

void USlashOverlay::SetGold(int32 Gold)
{
	if (GoldCountText)
		GoldCountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Gold)));
}

void USlashOverlay::SetSouls(int32 Souls)
{
	if (SoulsCountText) 
		SoulsCountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Souls)));
}

void USlashOverlay::SetPotionCount(int32 PotionAmmount)
{
	if (PotionCountText)
		PotionCountText->SetText(FText::FromString(FString::Printf(TEXT("%d"), PotionAmmount)));
}

void USlashOverlay::SetCrosshairVisibility(ESlateVisibility SlateVisiblity)
{
	if (Crosshair) Crosshair->SetVisibility(SlateVisiblity);
}
