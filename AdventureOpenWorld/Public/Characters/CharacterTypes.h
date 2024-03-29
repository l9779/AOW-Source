#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon"),
	ECS_EquippedBow UMETA(DisplayName = "Equipped Bow")
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_HitReaction UMETA(DisplayName = "Hit Reaction"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon"),
	EAS_Dodge UMETA(DisplayName = "Dodge"),
	EAS_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum EDeathPose
{
	EDP_Death1 UMETA(DisplayName = "Death 1"),
	EDP_Death2 UMETA(DisplayName = "Death 2"),
	EDP_Death3 UMETA(DisplayName = "Death 3"),
	EDP_Death4 UMETA(DisplayName = "Death 4"),

	EDP_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_NoState UMETA(DisplayName = "No State"),

	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling  UMETA(DisplayName = "Patrolling"),
	EES_Chasing  UMETA(DisplayName = "Chasing"),
	EES_Attacking  UMETA(DisplayName = "Attacking"),
	EES_Engaged  UMETA(DisplayName = "Engaged"),
};