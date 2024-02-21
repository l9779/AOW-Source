#include "HUD/RadialBarComponent.h"
#include "HUD/RadialBar.h"

void URadialBarComponent::SetBarPercent(const float& Percent)
{
	// Tentar colocar todo o código em c++
	//  + Adicionar acão de armar/desarmar
	if (!RadialBarWidget) RadialBarWidget = Cast<URadialBar>(GetUserWidgetObject());

	if (RadialBarWidget) RadialBarWidget->SetBarPercent(Percent);
}
