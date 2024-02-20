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

	virtual void Tick(float DeltaTime) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override; /** <IHitInterface/> */

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	FVector GetRandomMoveToLocation();

private:
	/* Functionality */
	void InitializeSettings();
	void SetHealthBarPercent();
	void SetHealthBarVisibility(bool Visibility);
	FVector GetNextFleetingLocation();
	bool IsDead() const;
	bool IsInsideRange(FVector& Location, double Radius) const;
	void PlayHitSound(const FVector& ImpactLocation);
	void SpawnHitParticle(const FVector& ImpactLocation);
	void MoveToTimerFinished();

	/* Change Behavior */
	void SetFleetingMode();
	void SetRoamingMode();
	void SetDeadMode();
	void ForgetHunter();
	
	/* Actions */
	void Die();
	void MoveToLocation(const FVector& Location);
	
	/* Behavior */
	void RunAwayFromHunter();
	void RoamingBehavior();
	void CombatBehavior();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); // Callback for OnPawnSeen in UPawnSensingComponent

	EPreyStates PreyState;
	FTimerHandle PatrolTimer;
	TObjectPtr<class AAIController> EnemyController;
	TObjectPtr<AActor> Hunter;

	UPROPERTY(VisibleInstanceOnly, Category = "AI Navigation")
	FVector MoveToTranslation;
	/** Actor that prey will keep roaming around */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Navigation", meta = (AllowPrivateAccess = true))
	TObjectPtr<AActor> RoamingAreaPoint;
	/** Size of radius around around RoamingAreaPoint */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Navigation", meta = (AllowPrivateAccess = true))
	float PatrolAreaRadius;
	/** If inside this radius prey must keep fleeting from hunter */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Navigation", meta = (AllowPrivateAccess = true))
	float HunterDangerRadius;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Navigation", meta = (AllowPrivateAccess = true))
	float PatrolWaitMin;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI Navigation", meta = (AllowPrivateAccess = true))
	float PatrolWaitMax;

	/** Radius used in IsInsideRange when roaming */
	UPROPERTY(EditAnywhere, Category = "AI Navigation", meta = (AllowPrivateAccess = true))
	float PatrolAcceptanceRadius;
	/** Radius used in IsInsideRange when fleeting, 
	* larger than PatrolAcceptanceRadius for more jittery move. */
	UPROPERTY(EditAnywhere, Category = "AI Navigation", meta = (AllowPrivateAccess = true))
	float CombatFleetAcceptanceRadius;
	/** Radius of Move to function, should be smaller than PatrolAcceptanceRadius */
	UPROPERTY(EditAnywhere, Category = "AI Navigation", meta = (AllowPrivateAccess = true))
	float MoveToLocationRadius;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UAttributeComponent> Attributes;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UHealthBarComponent> HealthBarWidgetComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UPawnSensingComponent> PawnSensing;

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

public:
	UFUNCTION(BlueprintCallable)
	EPreyStates GetPreyStates() const { return PreyState; }

};
