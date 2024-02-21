#include "HUD/RadialBarComponent.h"
#include "HUD/RadialBar.h"
#include "Components/Image.h"

void URadialBarComponent::SetBarPercent(const float& Percent)
{
	if (!RadialBarWidget) RadialBarWidget = Cast<URadialBar>(GetUserWidgetObject());

	if (RadialBarWidget && RadialBarMaterialInstance)
	{
		if (!DynamicRadialBarMaterialInstance)
			DynamicRadialBarMaterialInstance = CreateDynamicMaterialInstance(0, RadialBarMaterialInstance);
		
		DynamicRadialBarMaterialInstance->SetScalarParameterValue(FName("Percentage"), Percent);	
		RadialBarWidget->BarImage->SetBrushFromMaterial(DynamicRadialBarMaterialInstance);
	}
}
