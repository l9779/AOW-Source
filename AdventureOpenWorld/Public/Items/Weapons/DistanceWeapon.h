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

	virtual void Attack() override;
	void FireArrow();

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnArrow(FTransform SpawnTransform);

private:
	FTransform GetArrowSpawnTransform(FVector& CrosshairWorldLocation, FVector& CrosshairImpactPoint) const;

	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* SkeletalMesh;
	
};
