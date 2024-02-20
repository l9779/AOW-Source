#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
//#include "Characters/CharacterTypes.h"
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

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override; /* <IHitInterface/> */

	/* End of Public */
protected:
	virtual void BeginPlay() override; /** <AActor/> */

	/** <ABaseCharacter> */
	virtual void Die_Implementation() override;
	void SpawnSoul();
	virtual void Attack() override;
	virtual bool CanAttack() const override;
	virtual void ANCB_AttackEnd() override;
	virtual void HandleDamage(float DamageAmount) override;
	/** </ABaseCharacter> */

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Combat)
	EEnemyState EnemyState;
		
	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class ASoul> SoulClass;
	UPROPERTY(EditAnywhere, Category = Combat)
	FName WeaponSocket = FName("");

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
	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY()
	TObjectPtr<class AAIController> EnemyController;

	/** Will chase CombatTarget inside this radius */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	double CombatRadius;
	/** Radius to start attack */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	double AttackRadius;

	/** Navigation */
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TObjectPtr<AActor> PatrolTarget;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<TObjectPtr<AActor>> PatrolTargets;
	/** Radius of patrol points, if inside radius -> pick next patrol point */
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	double PatrolRadius;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax;
	FTimerHandle PatrolTimer;

	/** Acceptance radius of move to target, should be smaller than AttackRadius */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float AcceptanceRadius;

	FTimerHandle AttackTimer;
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackWaitMin;
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackWaitMax;
	UPROPERTY(EditAnywhere, Category = Combat)
	float DeathLifeSpan;

	/* End of Private */
};
