#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Items/Weapons/WeaponTypes.h"
#include "Interfaces/CollectableInterface.h"
#include "Weapon.generated.h"

UCLASS()
class ADVENTUREOPENWORLD_API AWeapon : public AItem, public ICollectableInterface
{
	GENERATED_BODY()

public:
	AWeapon();

	virtual void Tick(float DeltaTime) override;

	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator, bool bPlaySound = true);
	virtual void Unequip();
	virtual void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	/*  <ICollectableInterface> */
	virtual void SetIsBeignPickedUp(bool PickedUp) override;
	virtual void SetBarPercent(const float& Percent) override;
	/* /<ICollectableInterface> */

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

	UPROPERTY(VisibleAnywhere, Category = "Radial Bar")
	TObjectPtr<class URadialBarComponent> RadialBarComponentComponent;
	
	/** Pickup Item Variables */
	bool bIsBeignPickedUp = false;
	bool bCanBePickepUp = false;

	float PickupFill = 0.f;
	/** Time necessary to hold pickup key for item*/
	UPROPERTY(EditDefaultsOnly, Category = "Collectable Properties")
	float PickupTime = 1.5f;

public:
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE float GetStaminaAttackCost() const { return StaminaAttackCost; }
	FORCEINLINE bool CanBeEquipped() const { return bCanBePickepUp; }
};
