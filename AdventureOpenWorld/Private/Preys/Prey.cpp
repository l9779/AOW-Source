#include "Preys/Prey.h"
#include "AIController.h"
#include "Components/AttributeComponent.h"
#include "Components/SphereComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"

#include "AdventureOpenWorld/DebugMacros.h"

APrey::APrey():
	PatrolRadius(2000.f), PatrolWaitMin(5.f), PatrolWaitMax(10.f)
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

	PatrolSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Patrol Sphere"));
	PatrolSphere->SetSphereRadius(PatrolRadius);
	PatrolSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PatrolSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Overlap);

	HealthBarWidgetComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("Health Bar"));
	HealthBarWidgetComponent->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

}

void APrey::BeginPlay()
{
	Super::BeginPlay();
	
	EnemyController = Cast<AAIController>(GetController());

	InitializeSettings();

	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &APrey::PawnSeen);

	PatrolSphere->OnComponentBeginOverlap.AddDynamic(this, &APrey::OnSphereOverlap);
	PatrolSphere->OnComponentEndOverlap.AddDynamic(this, &APrey::OnSphereEndOverlap);
}

void APrey::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ATargetPoint>(OtherActor)) PatrolPoints.Add(OtherActor);
}

void APrey::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (Cast<ATargetPoint>(OtherActor)) PatrolPoints.Remove(OtherActor);
}

void APrey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DRAW_POINT_SingleFrame(GetActorLocation());

	// Seen Hunter || Hit by Hunter ?
	// - PreyState = Fleeting
	// - MaxWalkSpeed = RunSpeed
	// - Run Away From Hunter()
	// 
	// Got Away From Hunter ?
	// - PreyState = Roaming
	// - MaxWalkSpeed = WalkSpeed
	// - Back to Roaming()
	// 
	// Roaming ? 
	// - ReachToPatrolPoint()
	// - Wait
	// - GetNextPatrolPoint()
	// - MoveToPatrolPoint()
}

float APrey::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes) Attributes->ReceiveDamage(DamageAmount);

	return DamageAmount;
}

void APrey::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (!Hunter) Hunter = Hitter;
	
	SetHealthBarPercent();
	PlayHitSound(ImpactPoint);
	SpawnHitParticle(ImpactPoint);

	if (IsAlive())
	{
		SetHealthBarVisibility(true);
		//HitReact();
	} 
	else
	{
		SetHealthBarVisibility(false);
		Die();
	}
}

void APrey::InitializeSettings()
{
	if (Attributes) GetCharacterMovement()->MaxWalkSpeed = Attributes->GetWalkSpeed();
	SetHealthBarPercent();
	SetHealthBarVisibility(false);
	PreyState = EPreyStates::EPS_Roaming;
}

void APrey::SetHealthBarPercent()
{
	if (HealthBarWidgetComponent && Attributes)
		HealthBarWidgetComponent->SetHealthPercent(Attributes->GetHealthPercent());
}

void APrey::SetHealthBarVisibility(bool Visibility)
{
	if (HealthBarWidgetComponent) HealthBarWidgetComponent->SetVisibility(Visibility);
}

bool APrey::IsAlive() const
{
	return Attributes && Attributes->IsAlive();
}

void APrey::Die()
{
	SetHealthBarVisibility(false);
	PreyState = EPreyStates::EPS_Dead;
	// PlayDeathMontage();
}

void APrey::PlayHitSound(const FVector& ImpactLocation)
{
	if (HitSound) UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactLocation);
}

void APrey::SpawnHitParticle(const FVector& ImpactLocation)
{
	if (HitParticle) UGameplayStatics::SpawnEmitterAtLocation(this, HitParticle, ImpactLocation);
}

void APrey::PawnSeen(APawn* SeenPawn)
{
	if (SeenPawn->ActorHasTag(FName("Dead")) || Hunter) return;

	if (SeenPawn->ActorHasTag(FName("PlayerCharacter")))
	{
		Hunter = SeenPawn;
		PreyState = EPreyStates::EPS_Fleeting;
		UE_LOG(LogTemp, Warning, TEXT("ICUY"));
		// run away mode()
	}
}

