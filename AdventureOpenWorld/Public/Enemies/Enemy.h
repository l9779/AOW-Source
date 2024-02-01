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
	virtual void Tick(float DeltaTime) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

	virtual void Die() override;

	bool InTargetRange(AActor* Target, double Radius) const;
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EDeathPose DeathPose;

private:
	/** Components */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UHealthBarComponent> HealthBarWidgetComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UPawnSensingComponent> PawnSensing;

	UPROPERTY()
	TObjectPtr<AActor> CombatTarget;
	UPROPERTY(EditAnywhere)
	double CombatRadius;
	UPROPERTY(EditAnywhere)
	double AttackRadius;

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
	UPROPERTY()
	TObjectPtr<class AAIController> EnemyController;	
	
	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	void CheckPatrolTarget();
	void CheckCombatTarget();

	EEnemyState EnemyState;
	
};
