#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;

UCLASS()
class ADVENTUREOPENWORLD_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	/*
	*  AnimNotify callbacks
	*/
	UFUNCTION(BlueprintCallable)
	void ANCB_SetWeaponBoxCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
	UFUNCTION(BlueprintCallable)
	virtual void ANCB_AttackEnd();

protected:
	virtual void BeginPlay() override;

	virtual void Attack();
	virtual void Die();
	void DirectionalHitReact(const FVector& ImpactPoint);

	/*
	* Play Montage Functions
	*/
	virtual void PlayAttackMontage();
	void PlayHitReactMontage(const FName& SectionName);


	virtual bool CanAttack() const;

	/*
	* Animation Montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> OneHandSwordAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> GreatSwordAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TObjectPtr<AWeapon> EquippedWeapon;

	/*
	* Components
	*/
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UAttributeComponent> Attributes;
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<USoundBase> HitSound;
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UParticleSystem> HitParticles;
};
