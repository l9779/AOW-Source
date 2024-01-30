#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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
class AWeapon;

UCLASS()
class ADVENTUREOPENWORLD_API ASlashCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASlashCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

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

	/*
	* Animation Montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> OneHandSwordAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> GreatSwordAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> EquipMontage;

	/*
	* Input callbacks
	*/
	void Movement(const FInputActionValue& Value);
	void ClearMovement(const FInputActionValue& Value);
	void LookAround(const FInputActionValue& Value);
	void Walk();
	void EKeyPressed();
	void Attack();

	/*
	* Play Montage Functions
	*/
	void PlayAttackMontage();
	void OrientAttackRotation(float DeltaTime);
	void PlayEquipMontage(const FName& SectionName);
	
	/*
	*  AnimNotify callbacks
	*/
	UFUNCTION(BlueprintCallable)
	void ANCB_AttackEnd();
	UFUNCTION(BlueprintCallable)
	void ANCB_Disarm();
	UFUNCTION(BlueprintCallable)
	void ANCB_Arm();
	UFUNCTION(BlueprintCallable)
	void ANCB_SetWeaponBoxCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
	UFUNCTION(BlueprintCallable)
	void ANCB_SetDirectionAttack(bool b);

	bool CanAttack() const;
	bool CanDisarm() const;
	bool CanArm() const;
	
private:	
	/*
	* Dev Only
	*/
	UPROPERTY(EditAnywhere, Category = "Development Only Settings", meta = (AllowPrivateAccess = "true"))
	bool AttackMontageOvewrite = false;
	UPROPERTY(EditAnywhere, Category = "Development Only Settings", meta = (AllowPrivateAccess = "true"))
	FName AttackSectionName = FName("");
	
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;
	/* Set by anim notify */
	bool OrientAttackToRotation = false;

	bool WalkMode = false;
	UPROPERTY(EditAnywhere, Category = Movement)
	float WalkSpeed = 300.f;
	UPROPERTY(EditAnywhere, Category = Movement)
	float JogSpeed = 600.f;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = "Hair Groom")
	TObjectPtr<UGroomComponent> HeadHair;
	UPROPERTY(VisibleAnywhere, Category = "Hair Groom")
	TObjectPtr<UGroomComponent> EyebrownHair;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AItem> OverlappingItem;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TObjectPtr<AWeapon> EquippedWeapon;

	float InputY = 0.f;
	float InputX = 0.f;

public:
	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
};
