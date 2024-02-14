#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Items/Weapons/WeaponTypes.h"
#include "Weapon.generated.h"

UCLASS()
class ADVENTUREOPENWORLD_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator, bool bPlaySound = true);
	void Unequip();
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

protected:
	//UFUNCTION(BlueprintImplementableEvent)
	//void CreateFields(const FVector& FieldLocation);  transfered to breakable actor
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties|Meelee")
	float StaminaAttackCost;

private:
	void DeactivateEmbers();
	void DisableSphereCollision();
	void PlayEquipSound();

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<USoundBase> EquipSound;

public:
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE float GetStaminaAttackCost() const { return StaminaAttackCost; }
};
