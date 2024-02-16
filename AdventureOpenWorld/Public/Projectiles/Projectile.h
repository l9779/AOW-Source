#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class ADVENTUREOPENWORLD_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	void BeginPlay() override;
	void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> RootScene;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<class UBoxComponent> BoxVolume;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovementComponent;

private:
	void AttachArrow(AActor* OtherActor);

	UPROPERTY(EditDefaultsOnly)
	float Speed;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	float Damage;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UParticleSystem> HitParticle;

	TArray<TObjectPtr<AActor>> ActorsToIgnore;

public:
	FORCEINLINE void SetDamage(float BowDamage) { Damage = BowDamage; }
};
