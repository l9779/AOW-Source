#pragma once

#include "CoreMinimal.h"
#include "Actors/Item.h"
#include "Weapon.generated.h"

UCLASS()
class ADVENTUREOPENWORLD_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	void Equip(USceneComponent* InParent, FName InSocketName);

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

};
