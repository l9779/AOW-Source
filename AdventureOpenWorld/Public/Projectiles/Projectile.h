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
	USceneComponent* RootScene;
	UPROPERTY(EditDefaultsOnly)
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* BoxVolume;
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	class UProjectileMovementComponent* ProjectileMovementComponent;

private:
	UPROPERTY(EditDefaultsOnly)
	float Speed;
	
	float Damage;
	
	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* HitParticle;

	TArray<AActor*> ActorsToIgnore;

public:
	FORCEINLINE void SetDamage(float BowDamage) { Damage = BowDamage; }
};
