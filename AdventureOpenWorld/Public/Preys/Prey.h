// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Prey.generated.h"

UENUM(BlueprintType)
enum class EPreyStates : uint8
{
	EPS_Dead UMETA(DisplayName = "Dead"),
	EPS_Roaming UMETA(DisplayName = "Roaming"),
	EPS_Fleeting UMETA(DisplayName = "Fleeting")
};

/**
*	Essa classe poderia herdar de AEnemy,
	já que as duas classes tem muitas funcionalidades em comum.
	Eu escolhi refazer do zero para praticar e porquê eu posso refazer AEnemy no futuro.
*/

UCLASS()
class ADVENTUREOPENWORLD_API APrey : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	APrey();

	UFUNCTION(BlueprintCallable)
	EPreyStates GetPreyStates() const { return PreyState; }

protected:
	virtual void BeginPlay() override;

	void PlayAnimMontage(UAnimMontage* AnimMontage, float PlayRate = 1.f) const;

public:
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override; /** <IHitInterface/> */

	UFUNCTION(BlueprintImplementableEvent)
	FVector GetRandomMoveToLocation();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Navigation")
	TObjectPtr<AActor> RoamingAreaPoint;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "AI Navigation")
	FVector MoveToTranslation;

private:
	void InitializeSettings();
	void SetHealthBarPercent();
	void SetHealthBarVisibility(bool Visibility);
	bool IsDead() const;
	void Die();
	void PlayHitSound(const FVector& ImpactLocation);
	void SpawnHitParticle(const FVector& ImpactLocation);
	void SetFleetingMode();
	void SetRoamingMode();
	void SetDeadMode();
	void MoveToLocation(const FVector& Location);
	bool IsInsideRange(FVector& Location, double Radius) const;
	void MoveToTimerFinished();
	void ForgetHunter();
	void RunAwayFromHunter();
	void RoamingBehavior();
	void CombatBehavior();
	FVector GetNextFleetingLocation();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); // Callback for OnPawnSeen in UPawnSensingComponent

	UPROPERTY()
	EPreyStates PreyState;

	/** If inside this radius prey must keel fleeting from hunter */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "AI Navigation")
	float HunterDangerRadius;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "AI Navigation")
	float PatrolAreaRadius;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true), Category = "AI Navigation")
	float PatrolWaitMin;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true), Category = "AI Navigation")
	float PatrolWaitMax;

	/** Radius used in IsInsideRange when roaming */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true), Category = "AI Navigation")
	float PatrolAcceptanceRadius;
	/** Radius used in IsInsideRange when fleeting, 
	* larger than PatrolAcceptanceRadius for more jitery move.*/
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true), Category = "AI Navigation")
	float CombatFleetAcceptanceRadius;

	/** Radius of Move to function, should be smaller than PatrolAcceptanceRadius */
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true), Category = "AI Navigation")
	float MoveToLocationRadius;

	FTimerHandle PatrolTimer;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UAttributeComponent> Attributes;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UHealthBarComponent> HealthBarWidgetComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UPawnSensingComponent> PawnSensing;

	UPROPERTY()
	TObjectPtr<class AAIController> EnemyController;

	TObjectPtr<AActor> Hunter;



	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UParticleSystem> HitParticle; 
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> IdleMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<UAnimMontage> DeathMontage;
};
