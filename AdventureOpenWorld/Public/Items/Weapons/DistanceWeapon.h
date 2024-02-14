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

private:
	UPROPERTY(EditDefaultsOnly)
	USkeletalMeshComponent* SkeletalMesh;
	
};
