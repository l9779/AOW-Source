#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"

void ASlashHUD::BeginPlay()
{	
	Super::BeginPlay();

	if (SlashOverlayClass && GetWorld())
	{
		if (APlayerController* Controller = GetWorld()->GetFirstPlayerController())
		{
			SlashOverlay = CreateWidget<USlashOverlay>(Controller, SlashOverlayClass);
			SlashOverlay->AddToViewport();
		}
	}
}
