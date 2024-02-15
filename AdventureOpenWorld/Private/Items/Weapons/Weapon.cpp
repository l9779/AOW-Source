#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"

AWeapon::AWeapon(): 
	Damage(20.f), StaminaAttackCost(25.f),
	WeaponType(EWeaponType::EWT_Default)
{

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
}

void AWeapon::Attack()
{
}

void AWeapon::Unequip()
{
	SetOwner(nullptr);
	SetInstigator(nullptr);

	ItemState = EItemState::EIS_Hovering;

	if (Sphere) Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
