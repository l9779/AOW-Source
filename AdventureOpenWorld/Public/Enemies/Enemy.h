#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

UCLASS()
class ADVENTUREOPENWORLD_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/** </AActor> */

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override; 	/** <IHitInterface/> */

	/* End of Public */
protected:
	virtual void BeginPlay() override; /** <AActor/> */

	/** <ABaseCharacter> */
	virtual void Die() override;
	virtual void Attack() override;
	virtual bool CanAttack() const override;
	virtual void ANCB_AttackEnd() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual int32 PlayDeathMontage() override;
	/** </ABaseCharacter> */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<EDeathPose> DeathPose;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EEnemyState EnemyState;

	/* End of Protected */
private:
	/** Ai Behavior */
	void InitializeEnemy();
	void SpawnDefaultWeapon();
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void PatrolTimerFinished();
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius() const;
	bool IsOutsideAttackRadius() const;
	bool IsInsideAttackRadius() const;
	bool IsChasing() const;
	bool IsAttacking() const;
	bool IsDead() const;
	bool IsEngaged() const;
	void ClearPatrolTimer();
	void StartAttackTimer();
	void ClearAttackTimer();
	bool InTargetRange(AActor* Target, double Radius) const;
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); // Callback for OnPawnSeen in UPawnSensingComponent

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UHealthBarComponent> HealthBarWidgetComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UPawnSensingComponent> PawnSensing;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY()
	TObjectPtr<AActor> CombatTarget;
	UPROPERTY(EditAnywhere)
	double CombatRadius;
	UPROPERTY(EditAnywhere)
	double AttackRadius;

	UPROPERTY()
	TObjectPtr<class AAIController> EnemyController;

	/** Navigation */
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TObjectPtr<AActor> PatrolTarget;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<TObjectPtr<AActor>> PatrolTargets;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double PatrolRadius;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax;
	FTimerHandle PatrolTimer;

	FTimerHandle AttackTimer;
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackWaitMin;
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackWaitMax;

	UPROPERTY(EditAnywhere, Category = Combat)
	float DeathLifeSpan;

	/* End of Private */
};
