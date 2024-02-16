// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Prey.generated.h"

UENUM(BlueprintType)
enum class EPreyStates : uint8
{
	EPS_Roaming UMETA(DisplayName = "Roaming"),
	EPS_Fleeting UMETA(DisplayName = "Fleeting"),
	EPS_Dead UMETA(DisplayName = "Dead")
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

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
public:
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override; /** <IHitInterface/> */

private:
	void InitializeSettings();
	void SetHealthBarPercent();
	void SetHealthBarVisibility(bool Visibility);
	bool IsAlive() const;
	void Die();
	void PlayHitSound(const FVector& ImpactLocation);
	void SpawnHitParticle(const FVector& ImpactLocation);

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); // Callback for OnPawnSeen in UPawnSensingComponent

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true), Category = "AI Navigation")
	float PatrolRadius;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true), Category = "AI Navigation")
	float PatrolWaitMin;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true), Category = "AI Navigation")
	float PatrolWaitMax;

	UPROPERTY(VisibleInstanceOnly, Category = "AI Navigation")
	TArray<TObjectPtr<AActor>> PatrolPoints;
	UPROPERTY(VisibleInstanceOnly, Category = "AI Navigation")
	TObjectPtr<AActor> PatrolPoint;

	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<class UAttributeComponent> Attributes;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class USphereComponent> PatrolSphere;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UHealthBarComponent> HealthBarWidgetComponent;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<class UPawnSensingComponent> PawnSensing;

	UPROPERTY()
	TObjectPtr<class AAIController> EnemyController;

	TObjectPtr<AActor> Hunter;

	UPROPERTY(VisibleInstanceOnly)
	EPreyStates PreyState;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UParticleSystem> HitParticle; 
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<class UAnimMontage> HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<class UAnimMontage> IdleMontage;
	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	TObjectPtr<class UAnimMontage> DeathMontage;
};
