#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPickupInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for actors that will overlap with pickupable items
 */
class ADVENTUREOPENWORLD_API IPickupInterface
{
	GENERATED_BODY()

public:
	virtual void SetOverlappingItem(class AItem* Item);
	virtual void SetOverlappingLootable(class ALootableSupplies* Lootable);
	virtual void AddSouls(class ASoul* Soul);
	virtual void AddGold(class ATreasure* Gold);
	virtual bool CollectHealthPotion();
};
