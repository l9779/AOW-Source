#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_EquippingWeapon  UMETA(DisplayName = "Equipping Weapon"),
	EAS_Unoccupied  UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking  UMETA(DisplayName = "Attacking")
};

UENUM(BlueprintType)
enum class EDeathPose : uint8
{
	EDP_Alive UMETA(DisplayName = "Alive"), 
	EDP_Death1 UMETA(DisplayName = "Death 1"),
	EDP_Death2 UMETA(DisplayName = "Death 2"),
	EDP_Death3 UMETA(DisplayName = "Death 3"),
	EDP_Death4 UMETA(DisplayName = "Death 4"),
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Patrolling  UMETA(DisplayName = "Patrolling"),
	EES_Chasing  UMETA(DisplayName = "Chasing"),
	EES_Attacking  UMETA(DisplayName = "Attacking")
};