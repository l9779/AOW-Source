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

protected:
	virtual void BeginPlay() override;

	/** Combat Functions */
	virtual void Attack();
	virtual void Die();
	void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	void DisableCapsule();
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	virtual bool CanAttack() const;
	bool IsAlive() const;

	/** Play Montage Functions */
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	void PlayHitReactMontage(const FName& SectionName);	

	/**  AnimNotify callbacks */
	UFUNCTION(BlueprintCallable)
	virtual void ANCB_AttackEnd();
	UFUNCTION(BlueprintCallable)
	void ANCB_SetWeaponBoxCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	TObjectPtr<AWeapon> EquippedWeapon;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UAttributeComponent> Attributes;

	/* End of Protected */
private:
	UPROPERTY(EditAnywhere, Category = "Combat Assets")
	TObjectPtr<USoundBase> HitSound;
	UPROPERTY(EditAnywhere, Category = "Combat Assets")
	TObjectPtr<UParticleSystem> HitParticles;

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

	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> AttackMontageSections;
	UPROPERTY(EditAnywhere, Category = "Combat")
	TArray<FName> DeathMontageSections;

	/* End of Private */
};
