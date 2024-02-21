#include "HUD/RadialBarComponent.h"
#include "HUD/RadialBar.h"

void URadialBarComponent::SetBarPercent(const float& Percent)
{
	// Tentar colocar todo o c�digo em c++
	//  + Adicionar ac�o de armar/desarmar
	if (!RadialBarWidget) RadialBarWidget = Cast<URadialBar>(GetUserWidgetObject());

	if (RadialBarWidget) RadialBarWidget->SetBarPercent(Percent);
}
