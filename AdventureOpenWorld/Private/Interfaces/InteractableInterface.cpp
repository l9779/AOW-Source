#include "Interfaces/InteractableInterface.h"

// Add default functionality here for any IInteractableInterface functions that are not pure virtual.

void IInteractableInterface::SetIsBeignActivated(bool IsInteracting)
{
	
	bIsBeignActivated = IsInteracting;

	if (!IsInteracting) 
	{
		InteractionFill = 0.f;
		bInteractionConcluded = false;
	} 
}

void IInteractableInterface::SetBarPercent(const float& Percent, float DeltaTime)
{
	InteractionFill = FMath::FInterpConstantTo(InteractionFill, InteractionTime, DeltaTime, 1.f);
	if (InteractionFill == InteractionTime) bInteractionConcluded = true;
}

void IInteractableInterface::OnInteractionConcluded()
{
}
