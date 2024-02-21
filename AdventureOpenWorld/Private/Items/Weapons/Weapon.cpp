#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "HUD/RadialBarComponent.h"

AWeapon::AWeapon(): 
	Damage(20.f), StaminaAttackCost(25.f),
	WeaponType(EWeaponType::EWT_Default)
{
	PrimaryActorTick.bCanEverTick = true;

	RadialBarComponentComponent = CreateDefaultSubobject<URadialBarComponent>(TEXT("Radial Bar"));
	RadialBarComponentComponent->SetupAttachment(GetRootComponent());
	RadialBarComponentComponent->SetVisibility(false);
	RadialBarComponentComponent->SetDrawSize(FVector2D(80.f, 80.f));
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsBeignPickedUp)
	{
		PickupFill = FMath::FInterpConstantTo(PickupFill, PickupTime, DeltaTime, 1.f);
		SetBarPercent(PickupFill / PickupTime);
		if (PickupFill == PickupTime) bCanBePickepUp = true;
	}

	if (GetActorLocation().Z > DesiredZ)
		AddActorWorldOffset(FVector(0.f, 0.f, 1.f * DeltaTime));
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator, bool bPlaySound)
{
	ItemState = EItemState::EIS_Equipped;
	AttachMeshToSocket(InParent, InSocketName);
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	DisableSphereCollision();

	if (bPlaySound) PlayEquipSound();
	DeactivateEmbers();
}

void AWeapon::DeactivateEmbers()
{
	if (ItemEffect) ItemEffect->Deactivate();
}

void AWeapon::DisableSphereCollision()
{
	if (Sphere) Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound)
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	// Works different on MeeleeWeapon and DistanceWeapon
}

void AWeapon::SetIsBeignPickedUp(bool PickedUp)
{
	bIsBeignPickedUp = PickedUp;
	if (RadialBarComponentComponent) RadialBarComponentComponent->SetVisibility(PickedUp);
	
	bCanBePickepUp = false;

	if (!PickedUp) PickupFill = 0.f;
	
}

void AWeapon::SetBarPercent(const float& Percent)
{
	if (RadialBarComponentComponent) RadialBarComponentComponent->SetBarPercent(Percent);
}

void AWeapon::Unequip()
{
	SetOwner(nullptr);
	SetInstigator(nullptr);

	ItemState = EItemState::EIS_Hovering;
	SetActorRotation(FRotator::ZeroRotator);
	DesiredZ = GetDesiredZ();

	if (Sphere) Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
