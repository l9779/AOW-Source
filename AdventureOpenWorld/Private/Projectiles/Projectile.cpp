#include "Projectiles/Projectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Interfaces/HitInterface.h"

AProjectile::AProjectile():
	Speed(6500.f)
{
	PrimaryActorTick.bCanEverTick = false;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("Root Componenet"));
	RootComponent = RootScene;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	Mesh->SetupAttachment(GetRootComponent());

	BoxVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Volume"));
	BoxVolume->SetupAttachment(Mesh);
	BoxVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxVolume->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
}

void AProjectile::OnConstruction(const FTransform& Transform)
{
	ProjectileMovementComponent->InitialSpeed = Speed;
	ProjectileMovementComponent->MaxSpeed = Speed;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(GetOwner());
	ActorsToIgnore.Add(GetInstigator());
	
	BoxVolume->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::OnBoxOverlap);
}

void AProjectile::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorsToIgnore.Contains(OtherActor)) return;

	ProjectileMovementComponent->StopMovementImmediately();
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
	AttachToActor(OtherActor, FAttachmentTransformRules::KeepWorldTransform);
	BoxVolume->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (HitParticle)
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), 
			HitParticle, 
			BoxVolume->GetComponentLocation());
}
