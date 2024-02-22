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

	RadialBarComponent = CreateDefaultSubobject<URadialBarComponent>(TEXT("Radial Bar"));
	RadialBarComponent->SetupAttachment(GetRootComponent());
	RadialBarComponent->SetVisibility(false);
	RadialBarComponent->SetDrawSize(FVector2D(80.f, 80.f));
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	InteractionTime = WeaponPickupTime;

}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsBeignActivated) SetBarPercent(InteractionFill / InteractionTime, DeltaTime);

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

void AWeapon::SetIsBeignActivated(bool IsInteracting)
{
	IInteractableInterface::SetIsBeignActivated(IsInteracting);

	if (RadialBarComponent) RadialBarComponent->SetVisibility(IsInteracting);
}

void AWeapon::SetBarPercent(const float& Percent, float DeltaTime)
{
	IInteractableInterface::SetBarPercent(Percent, DeltaTime);

	if (RadialBarComponent) RadialBarComponent->SetBarPercent(Percent);
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
