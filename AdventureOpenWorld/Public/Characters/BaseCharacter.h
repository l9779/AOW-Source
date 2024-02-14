#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "BaseCharacter.generated.h"

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
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override; /** <IHitInterface/> */
	virtual void Attack();

	UFUNCTION(BlueprintNativeEvent)
	void Die();

	void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	void PlayHitSound(const FVector& ImpactPoint);
	void SpawnHitParticles(const FVector& ImpactPoint);
	virtual bool CanAttack() const;
	bool IsAlive() const;
	void DisableCapsule();
	void DisableMeshCollision(); 

	/** Play Montage Functions */
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);
	virtual int32 PlayAttackMontage(bool bIsHeavyAttack = false);
	virtual int32 PlayDeathMontage();
	void PlayHitReactMontage(const FName& SectionName);	
	void PlayDodgeMontage();
	void StopAttackMontage();

	/* < Called on BP_Enemy Tick > */
	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();
	UFUNCTION(BlueprintCallable) 
	FVector GetRotationWarpTarget();
	/* </ Called on BP_Enemy Tick > */

	/**  AnimNotify callbacks */
	UFUNCTION(BlueprintCallable)
	virtual void ANCB_AttackEnd();
	UFUNCTION(BlueprintCallable)
	virtual void ANCB_DodgeEnd();
	UFUNCTION(BlueprintCallable)
	void ANCB_SetWeaponBoxCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AActor> CombatTarget;
	UPROPERTY(EditAnywhere, Category = "Combat")
	double WarpTargetDistance;

	UPROPERTY(VisibleAnywhere, Category = "Combat|Weapon")
	TObjectPtr<class AWeapon> EquippedWeapon;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UAttributeComponent> Attributes;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "States")
	TEnumAsByte<EDeathPose> DeathPose;

	/* End of Protected */
private:
	UPROPERTY(EditAnywhere, Category = "Combat|Sound")
	TObjectPtr<USoundBase> HitSound;
	UPROPERTY(EditAnywhere, Category = "Combat|VFX")
	TObjectPtr<UParticleSystem> HitParticles;

	/** Animation Montages */
	UPROPERTY(EditDefaultsOnly, Category = "Montages|Attack")
	TObjectPtr<UAnimMontage> DefaultAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages|Attack")
	TObjectPtr<UAnimMontage> SwordAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages|Attack")
	TObjectPtr<UAnimMontage> GreatSwordAttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages|Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages|Combat")
	TObjectPtr<UAnimMontage> DeathMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages|Combat")
	TObjectPtr<UAnimMontage> DodgeMontage;

	UPROPERTY(EditAnywhere, Category = "Combat|Montage Name Sections")
	TArray<FName> AttackMontageSections;
	UPROPERTY(EditAnywhere, Category = "Combat|Montage Name Sections")
	TArray<FName> HeavyAttackMontageSections;
	UPROPERTY(EditAnywhere, Category = "Combat|Montage Name Sections")
	TArray<FName> DeathMontageSections;

	/* End of Private */
public:
	FORCEINLINE void SetCombatTarget(AActor* Target) { CombatTarget = Target; }

	FORCEINLINE TEnumAsByte<EDeathPose> GetDeathPose() const { return DeathPose; }
};
