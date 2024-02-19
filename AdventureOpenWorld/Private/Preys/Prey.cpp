#include "Preys/Prey.h"
#include "AIController.h"
#include "Components/AttributeComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
//#include "NavigationSystem.h"
//#include "AI/Navigation/NavigationTypes.h"

#include "AdventureOpenWorld/DebugMacros.h"

APrey::APrey():
	HunterDangerRadius(500.f),
	PatrolAreaRadius(1000.f),
	PatrolWaitMin(5.f), PatrolWaitMax(10.f),
	PatrolAcceptanceRadius(200.f), CombatFleetAcceptanceRadius(300.f),
	MoveToLocationRadius(50.f)
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

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
}

void APrey::PlayAnimMontage(UAnimMontage* AnimMontage, float PlayRate) const
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
		AnimInstance->Montage_Play(AnimMontage, PlayRate);
}

void APrey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RoamingAreaPoint)
		DRAW_SPHERE_SingleFrame(RoamingAreaPoint->GetActorLocation(), FColor::Green, PatrolAreaRadius)

	//DRAW_SPHERE_SingleFrame(MoveToTranslation, FColor::Red, 25.f);

	if (IsDead()) return;

	if (PreyState > EPreyStates::EPS_Dead)
	{
		if (Hunter)
			CombatBehavior();
		else
			RoamingBehavior();
	}
}

void APrey::CombatBehavior()
{
	FVector HunterLocation = Hunter->GetActorLocation();

	if (IsInsideRange(HunterLocation, HunterDangerRadius))
		RunAwayFromHunter();
	else
		ForgetHunter();
}

void APrey::RoamingBehavior()
{
	if (IsInsideRange(MoveToTranslation, PatrolAcceptanceRadius))
	{
		MoveToTranslation = GetRandomMoveToLocation();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &APrey::MoveToTimerFinished, WaitTime);
	}
}

float APrey::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);

		if (Attributes->IsAlive())
		{
			SetHealthBarVisibility(true);
			SetFleetingMode();
			if (HitReactMontage) PlayAnimMontage(HitReactMontage);
		}
		else
		{
			Die();
		}
	}

	return DamageAmount;
}

void APrey::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (!Hunter) Hunter = Hitter;
		
	SetHealthBarPercent();
	PlayHitSound(ImpactPoint);
	SpawnHitParticle(ImpactPoint);
}

/*
void APrey::GetRandomMoveToLocation()
{
	if (UNavigationSystemV1* Navigation = UNavigationSystemV1::CreateNavigationSystem(GetWorld()))
	{
		FNavLocation OutNavLocation;
		if (Navigation->GetRandomPointInNavigableRadius(RoamingAreaPoint->GetActorLocation(), PatrolAreaRadius, OutNavLocation))
			MoveToTranslation = OutNavLocation.Location;
	}
}
*/

FVector APrey::GetNextFleetingLocation()
{
	TArray<FVector> PossibleLocations;
	for (int32 i = 0; i < 5; i++) PossibleLocations.Add(GetRandomMoveToLocation());
	
	FVector FleetingLocation(0.f);
	float FleetingLocationSize = FleetingLocation.Size();

	for (const FVector& Location : PossibleLocations)
	{
		// Get PossibleLocation maginetude to hunter
		float DistanceToHunterSize = (Location - Hunter->GetActorLocation()).Size();
		
		// if PossibleLocation maginetude is greater than Fleeting maginetude
		// for the first loop will be always true
		if (DistanceToHunterSize > FleetingLocationSize)
		{
			FleetingLocation = Location;
			FleetingLocationSize = DistanceToHunterSize;
			// Final Fleeting location will alway be the one furthest alway from hunter
		}
	}

	return FleetingLocation;
}

void APrey::InitializeSettings()
{
	SetHealthBarPercent();
	SetRoamingMode();
	MoveToTranslation = GetRandomMoveToLocation();
	MoveToLocation(MoveToTranslation);
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

bool APrey::IsDead() const
{
	return PreyState == EPreyStates::EPS_Dead;
}

void APrey::Die()
{
	SetDeadMode();
	if (DeathMontage) PlayAnimMontage(DeathMontage);
}

void APrey::PlayHitSound(const FVector& ImpactLocation)
{
	if (HitSound) UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactLocation);
}

void APrey::SpawnHitParticle(const FVector& ImpactLocation)
{
	if (HitParticle) UGameplayStatics::SpawnEmitterAtLocation(this, HitParticle, ImpactLocation);
}

void APrey::SetFleetingMode()
{
	PreyState = EPreyStates::EPS_Fleeting;
	if (Attributes) GetCharacterMovement()->MaxWalkSpeed = Attributes->GetRunSpeed();
}

void APrey::SetRoamingMode()
{
	PreyState = EPreyStates::EPS_Roaming;
	SetHealthBarVisibility(false);
	if (Attributes) GetCharacterMovement()->MaxWalkSpeed = Attributes->GetWalkSpeed();
}

void APrey::SetDeadMode()
{
	PreyState = EPreyStates::EPS_Dead;
	Hunter = nullptr;
	SetHealthBarVisibility(false);
}

void APrey::MoveToLocation(const FVector& Location)
{
	if (!EnemyController) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalLocation(Location);
	MoveRequest.SetAcceptanceRadius(MoveToLocationRadius);
	EnemyController->MoveTo(MoveRequest);
}

bool APrey::IsInsideRange(FVector& Location, double Radius) const
{
	return (Location - GetActorLocation()).Size() <= Radius;
}

void APrey::MoveToTimerFinished()
{
	MoveToLocation(MoveToTranslation);
}

void APrey::ForgetHunter()
{
	Hunter = nullptr;
	SetRoamingMode();
}

void APrey::RunAwayFromHunter()
{
	if (IsInsideRange(MoveToTranslation, CombatFleetAcceptanceRadius))
	{
		MoveToTranslation = GetNextFleetingLocation();
		MoveToLocation(MoveToTranslation);
	}
}

void APrey::PawnSeen(APawn* SeenPawn)
{
	if (SeenPawn->ActorHasTag(FName("Dead"))) return;

	if (!Hunter && SeenPawn->ActorHasTag(FName("PlayerCharacter")))
	{
		Hunter = SeenPawn;
		SetFleetingMode();
	}
}

