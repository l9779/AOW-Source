#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "SlashCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;

UCLASS()
class ADVENTUREOPENWORLD_API ASlashCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	ASlashCharacter();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override; 
	/** </AActor> */

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override; /** <IHitInterface/> */
	
	/** <IPickupInterface> */
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(class ASoul* Soul) override;
	virtual void AddGold(class ATreasure* Treasure) override;
	virtual bool CollectHealthPotion() override;
	/** </IPickupInterface> */

protected:
	virtual void BeginPlay() override;

	/** Input callbacks */
	void Movement(const FInputActionValue& Value);
	void ClearMovement(const FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);
	void Walk();
	void EquipKeyPressed();
	virtual void Jump() override; /** <ACharacter/> */
	virtual void Attack() override; /** <ABaseCharacter/> */
	void LeftShiftPressed();
	void LeftShiftReleased();
	void Dodge();
	void DrinkPotion();
	void RightMousePressed();
	void RightMouseReleased();

	/** Weapon and Combat Functions */
	/** <ABaseCharacter> */
	virtual bool CanAttack() const override; 
	virtual void Die_Implementation() override;
	/** </ABaseCharacter> */
	void OrientAttackRotation(float DeltaTime);
	void UnsheatWeapon();
	void SheatWeapon();
	bool CanDisarm() const;
	bool CanArm() const;
	void EquipWeapon(AWeapon* Weapon);
	bool HasEnoughStamina(float StaminaCost = 0.f) const;
	bool IsOccupied() const;

	/** AnimNotify callbacks */
	/** <ABaseCharacter> */
	virtual void ANCB_AttackEnd() override; 
	virtual void ANCB_DodgeEnd() override;
	/** <\ABaseCharacter> */
	UFUNCTION(BlueprintCallable)
	void ANCB_AttachWeaponToSheat();
	UFUNCTION(BlueprintCallable)
	void ANCB_AttachWeaponToHand();
	UFUNCTION(BlueprintCallable)
	void ANCB_SetDirectionAttack(bool b);
	UFUNCTION(BlueprintCallable)
	void ANCB_HitReactEnd();
	UFUNCTION(BlueprintCallable)
	void ANCB_SetPotionVisibility(bool Visiblity);
	UFUNCTION(BlueprintCallable)
	void ANCB_FireArrowEnd();

	/** Input Actions */
	UPROPERTY(EditAnywhere, Category = "Input|Mapping Context")
	TObjectPtr<UInputMappingContext> SlashMappingContext;	

	UPROPERTY(EditAnywhere, Category = "Input|Movement")
	TObjectPtr<UInputAction> MovementAction;
	UPROPERTY(EditAnywhere, Category = "Input|Movement")
	TObjectPtr<UInputAction> LookAroundAction;
	UPROPERTY(EditAnywhere, Category = "Input|Movement")
	TObjectPtr<UInputAction> WalkAction;
	UPROPERTY(EditAnywhere, Category = "Input|Movement")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input|Actions")
	TObjectPtr<UInputAction> EquipAction;
	UPROPERTY(EditAnywhere, Category = "Input|Actions")
	TObjectPtr<UInputAction> DrinkPotionAction;
	UPROPERTY(EditAnywhere, Category = "Input|Actions")
	TObjectPtr<UInputAction> DodgeAction;

	UPROPERTY(EditAnywhere, Category = "Input|Combat")
	TObjectPtr<UInputAction> AttackAction;
	UPROPERTY(EditAnywhere, Category = "Input|Combat")
	TObjectPtr<UInputAction> HeavyAttackAction;
	UPROPERTY(EditAnywhere, Category = "Input|Combat")
	TObjectPtr<UInputAction> RightMouseAction;

	/** Input Variables  */
	bool HoldingHeavyAttack = false; /* Set by input callback */
	bool WalkMode = false; /* Set by input callback */
	bool OrientAttackToRotation = false; /* Set by anim notify at the beginnig of attack animation */
	/* Set by pressing movement inputs, to orient attack rotation */
	float InputY = 0.f;
	float InputX = 0.f;
	/** Set true once fires arrow, set to false ANCB at end of fire arrow montage */
	bool FiringArrow = false;
	/** Set true if CharacterState == ECS_Equipped Bow and holding right mouse button */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Combat|Bow")
	bool bAimingBow = false; 

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UInventoryComponent> Inventory;

	/* End of Protected */
private:		
	bool IsUnoccupied() const;
	void SetCharacterStateOnWeapon();
	void InitializeSlashOverlay(APlayerController* PlayerController);
	void SetHUDPotionCount();
	void SetHUDHealth();
	void SetHUDStamina();
	bool CanFireArrow () const;

	/** Character Components */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = "Hair Groom")
	TObjectPtr<UGroomComponent> HeadHair;
	UPROPERTY(VisibleAnywhere, Category = "Hair Groom")
	TObjectPtr<UGroomComponent> EyebrownHair;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory", meta = (AllowPrivateAccess = true))
	TObjectPtr<UStaticMeshComponent> PotionMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> EquipMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages|Attack")
	TObjectPtr<UAnimMontage> FireBowMontage;

	/**	 Set by state of equipped weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "States", meta = (AllowPrivateAccess = true))
	ECharacterState CharacterState; 
	/** Set by different action character can peform(equipping weapon, attacks, unnocupied) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "States", meta = (AllowPrivateAccess = "true"))
	EActionState ActionState; 
	
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AItem> OverlappingItem;

	TObjectPtr<class USlashOverlay> SlashOverlay;

	/* End of Private */
public:
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
};
