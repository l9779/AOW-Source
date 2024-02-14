#pragma once

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Default UMETA(DisplayName = "DefaultWeapon"),
	EWT_OneHandSword UMETA(DisplayName = "One Hand Sword"),
	EWT_GreatSword UMETA(DisplayName = "Great Sword"),
	EWT_OneHandAxe UMETA(DisplayName = "One Hand Axe"),
	EWT_Bow UMETA(DisplayName = "Bow")
};