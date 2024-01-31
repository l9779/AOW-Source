#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UAnimMontage;

UCLASS()
class ADVENTUREOPENWORLD_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	void DirectionalHitReact(const FVector& ImpactPoint);
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

	void Die();

	bool InTargetRange(AActor* Target, double Radius) const;
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);	

	void PlayHitReactMontage(const FName& SectionName);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EDeathPose DeathPose;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = "VFX")
	TObjectPtr<UParticleSystem> HitParticles;

	/*
	* Components
	*/
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UAttributeComponent> Attributes;
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

	/*
	* Navigation
	*/
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
	
public:	
	
};
