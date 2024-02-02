#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterTypes.h"
#include "SlashCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class UGroomComponent;
class AItem;
class UAnimMontage;

UCLASS()
class ADVENTUREOPENWORLD_API ASlashCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ASlashCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override; /** <IHitInterface/> */

protected:
	virtual void BeginPlay() override;

	/** Input callbacks */
	void Movement(const FInputActionValue& Value);
	void ClearMovement(const FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);
	void Walk();
	void EKeyPressed();
	virtual void Attack() override; /** <ABaseCharacter/> */
	
	/** Weapon and Combat Functions */
	virtual bool CanAttack() const override; /** <ABaseCharacter/> */
	void OrientAttackRotation(float DeltaTime);
	void UnsheatWeapon();
	void SheatWeapon();
	bool CanDisarm() const;
	bool CanArm() const;
	void EquipWeapon(AWeapon* Weapon);

	/** AnimNotify callbacks */
	virtual void ANCB_AttackEnd() override; /** <ABaseCharacter/> */
	UFUNCTION(BlueprintCallable)
	void ANCB_AttachWeaponToSheat();
	UFUNCTION(BlueprintCallable)
	void ANCB_AttachWeaponToHand();
	UFUNCTION(BlueprintCallable)
	void ANCB_SetDirectionAttack(bool b);

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> SlashMappingContext;	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MovementAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAroundAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> WalkAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> EquipAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

	/* End of Protected */
private:		
	void SetCharacterStateOnWeapon();

	/** Character Components */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = "Hair Groom")
	TObjectPtr<UGroomComponent> HeadHair;
	UPROPERTY(VisibleAnywhere, Category = "Hair Groom")
	TObjectPtr<UGroomComponent> EyebrownHair;
	
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> EquipMontage;

	ECharacterState CharacterState; /* Set by state of equipped weapon */
	bool WalkMode = false; /* Set by input callback */
	bool OrientAttackToRotation = false; /* Set by anim notify at the beginnig of attack animation */
	/* Set by pressing movement inputs, to orient attack rotation */
	float InputY = 0.f;
	float InputX = 0.f;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState; /* Set by different action character can peform(equipping weapon, attacks, unnocupied) */
	
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AItem> OverlappingItem;

	/* End of Private */
public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
};
