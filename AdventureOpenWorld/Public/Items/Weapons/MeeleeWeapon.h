// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/Weapons/Weapon.h"
#include "MeeleeWeapon.generated.h"

/**
 * 
 */
UCLASS()
class ADVENTUREOPENWORLD_API AMeeleeWeapon : public AWeapon
{
	GENERATED_BODY()
public:
	AMeeleeWeapon();

	virtual void Unequip() override;
	virtual void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName) override;

	void ClearIgnoreActors();
protected:
	virtual void BeginPlay() override;

	UFUNCTION() 
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
	void ExecuteGetHit(FHitResult& OutBoxHit);
	bool HitActorIsSameType(AActor* OtherActor); 
	void BoxTrace(FHitResult& OutBoxHit);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties|Meelee")
	bool bShowBoxDebug = false;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties|Meelee")
	FVector BoxTraceExtent = FVector(5.f); 

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UBoxComponent> WeaponBox; 
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BoxTraceStart; 
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BoxTraceEnd;

	TArray<TObjectPtr<AActor>> IgnoreActors;
public:
	FORCEINLINE TObjectPtr<UBoxComponent> GetWeaponBox() { return WeaponBox; }
};
