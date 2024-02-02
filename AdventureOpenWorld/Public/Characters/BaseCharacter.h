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

	/**  AnimNotify callbacks */
	UFUNCTION(BlueprintCallable)
	void ANCB_SetWeaponBoxCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
	UFUNCTION(BlueprintCallable)
	virtual void ANCB_AttackEnd();

protected:
	virtual void BeginPlay() override;

	virtual void Attack();
	virtual void Die();
	void DirectionalHitReact(const FVector& ImpactPoint);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);

	/** Play Montage Functions */
	virtual void PlayAttackMontage();
	void PlayHitReactMontage(const FName& SectionName);


	virtual bool CanAttack() const;
	bool IsAlive() const;

	/** Animation Montages */
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> OneHandAxeAttackMontage;
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

	/** Components */
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UAttributeComponent> Attributes;
	

	/* Dev Only */
	UPROPERTY(EditAnywhere, Category = "Development Only Settings", meta = (AllowPrivateAccess = "true"))
	bool AttackMontageOvewrite = false;
	UPROPERTY(EditAnywhere, Category = "Development Only Settings", meta = (AllowPrivateAccess = "true"))
	FName AttackSectionName = FName("");

	/* End of Protected */
private:
	UPROPERTY(EditAnywhere, Category = "Combat Assets")
	TObjectPtr<USoundBase> HitSound;
	UPROPERTY(EditAnywhere, Category = "Combat Assets")
	TObjectPtr<UParticleSystem> HitParticles;
};
