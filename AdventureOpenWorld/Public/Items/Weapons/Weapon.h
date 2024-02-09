#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Characters/CharacterTypes.h"
#include "Weapon.generated.h"

class UBoxComponent;

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	EWT_Default UMETA(DisplayName = "DefaultWeapon"),
	EWT_OneHandSword UMETA(DisplayName = "One Hand Sword"),
	EWT_GreatSword UMETA(DisplayName = "Great Sword"),
	EWT_OneHandAxe UMETA(DisplayName = "One Hand Axe")
};

UCLASS()
class ADVENTUREOPENWORLD_API AWeapon : public AItem
{
	GENERATED_BODY()

public:
	AWeapon();

	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator, bool bPlaySound = true);
	void DeactivateEmbers();
	void DisableSphereCollision();
	void PlayEquipSound();
	void Unequip();
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	FORCEINLINE void ClearIgnoreActors() { IgnoreActors.Empty(); }

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	bool HitActorIsSameType(AActor* OtherActor);

	void ExecuteGetHit(FHitResult& OutBoxHit);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

private:
	void BoxTrace(FHitResult& OutBoxHit);

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f);
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<USoundBase> EquipSound;
	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<UBoxComponent> WeaponBox;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float StaminaAttackCost;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	EWeaponType WeaponType;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BoxTraceStart;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BoxTraceEnd;

	TArray<TObjectPtr<AActor>> IgnoreActors;

public:
	FORCEINLINE TObjectPtr<UBoxComponent> GetWeaponBox() { return WeaponBox; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE float GetStaminaAttackCost() const { return StaminaAttackCost; }
};
