#include "Preys/Prey.h"
#include "AIController.h"
#include "Components/AttributeComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "AI/Navigation/NavigationTypes.h"

#include "AdventureOpenWorld/DebugMacros.h"

APrey::APrey():
	SafeDistanceToHunter(500.f),
	PatrolAreaRadius(1000.f),
	PatrolWaitMin(5.f), PatrolWaitMax(10.f),
	PatrolAcceptanceRadius(200.f), MoveToLocationRadius(50.f)
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
	GetNextRoamingLocation();
	MoveToLocation(PatrolLocation);

	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &APrey::PawnSeen);
}

void APrey::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (RoamingAreaPoint)
		DRAW_SPHERE_SingleFrame(RoamingAreaPoint->GetActorLocation(), FColor::Green, PatrolAreaRadius)
	
	DRAW_SPHERE_SingleFrame(PatrolLocation, FColor::Red, 25.f)

	switch (PreyState)
	{
	case EPreyStates::EPS_Roaming:
		if (IsInsideRange(PatrolLocation, PatrolAcceptanceRadius))
		{
			GetNextRoamingLocation();
			const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
			GetWorldTimerManager().SetTimer(PatrolTimer, this, &APrey::PatrolTimerFinished, WaitTime);
		} 
		break;
	case EPreyStates::EPS_Fleeting:
		// Run Away from Hunter
		break;
	case EPreyStates::EPS_Dead:
		PrimaryActorTick.bCanEverTick = false;
		break;
	default:
		break;
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
			//HitReact();
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

void APrey::InitializeSettings()
{
	SetHealthBarPercent();
	SetRoamingMode();
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
	// PlayDeathMontage();
	SetDeadMode();
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

void APrey::PatrolTimerFinished()
{
	MoveToLocation(PatrolLocation);
}

void APrey::SetNextLocation()
{
	if (UNavigationSystemV1* Navigation = UNavigationSystemV1::CreateNavigationSystem(GetWorld()))
	{
		FNavLocation OutNavLocation;
		if (Navigation->GetRandomPointInNavigableRadius(RoamingAreaPoint->GetActorLocation(), PatrolAreaRadius, OutNavLocation))
			PatrolLocation = OutNavLocation.Location;
	}
}

void APrey::PawnSeen(APawn* SeenPawn)
{
	if (SeenPawn->ActorHasTag(FName("Dead"))) return;

	if (!Hunter && SeenPawn->ActorHasTag(FName("PlayerCharacter")))
	{
		Hunter = SeenPawn;
		SetFleetingMode();
		UE_LOG(LogTemp, Warning, TEXT("ICUY"));
	}
}

