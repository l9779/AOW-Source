#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Items/Weapons/WeaponTypes.h"
#include "Interfaces/InteractableInterface.h"
#include "Weapon.generated.h"

UCLASS()
class ADVENTUREOPENWORLD_API AWeapon : public AItem, public IInteractableInterface
{
	GENERATED_BODY()

public:
	AWeapon();

	virtual void Tick(float DeltaTime) override;

	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator, bool bPlaySound = true);
	virtual void Unequip();
	virtual void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	/*  <IInteractableInterface> */
	virtual void SetIsBeignActivated(bool IsInteracting) override;
	virtual void SetBarPercent(const float& Percent, float DeltaTime) override;
	/* /<IInteractableInterface> */

protected:
	virtual void BeginPlay() override;

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

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class URadialBarComponent> RadialBarComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Interactable Properties")
	float WeaponPickupTime = 1.5f;

public:
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE float GetStaminaAttackCost() const { return StaminaAttackCost; }
};
