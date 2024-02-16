// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/Weapon.h"
#include "DistanceWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTUREOPENWORLD_API ADistanceWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	ADistanceWeapon();

	virtual void Unequip() override;
	virtual void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName) override;

	void FireArrow();

	void StringPulled(bool isPulled);

protected:
	void SpawnArrow(FTransform SpawnTransform);

	UPROPERTY(BlueprintReadOnly)
	bool bStringPulled = false;
	UPROPERTY(BlueprintReadOnly)
	bool bIsAimed = false;
	
private:
	FTransform GetArrowSpawnTransform(FVector& CrosshairWorldLocation, FVector& CrosshairImpactPoint) const;
	void SetArrowVisibility(bool Visible);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AProjectile> ArrowClass;

public:
	FORCEINLINE void SetStringPulled(bool IsPulled) { bStringPulled = IsPulled; }
	FORCEINLINE void SetIsAimed(bool IsAimed) { bIsAimed = IsAimed; }
};
