// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/PickupInterface.h"

void IPickupInterface::SetOverlappingItem(AItem* Item)
{
}

void IPickupInterface::SetOverlappingLootable(ALootableSupplies* Lootable)
{
}

void IPickupInterface::AddSouls(ASoul* Soul)
{
}

void IPickupInterface::AddGold(ATreasure* Gold)
{
}

bool IPickupInterface::CollectHealthPotion()
{
	return false;
}
