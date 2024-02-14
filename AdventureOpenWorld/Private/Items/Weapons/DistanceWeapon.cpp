#include "Items/Weapons/DistanceWeapon.h"

ADistanceWeapon::ADistanceWeapon()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Skeletal Mesh"));
	RootComponent = SkeletalMesh;

	ItemMesh->SetupAttachment(GetRootComponent());
}
