#include "Enemies/Enemy.h"
#include "AIController.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "AdventureOpenWorld/DebugMacros.h"

AEnemy::AEnemy(): 
	DeathPose(EDeathPose::EDP_Alive), EnemyState(EEnemyState::EES_Patrolling),
	AttackRadius(120.f), CombatRadius(500.f),
	PatrolRadius(200.f), PatrolWaitMin(5.f), PatrolWaitMax(10.f)
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

	HealthBarWidgetComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("Health Bar"));
	HealthBarWidgetComponent->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->MaxWalkSpeed = 125.f;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("Pawn Sensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if (HealthBarWidgetComponent) HealthBarWidgetComponent->SetVisibility(false);

	EnemyController = Cast<AAIController>(GetController());

	MoveToTarget(PatrolTarget);

	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);

}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (EnemyState > EEnemyState::EES_Patrolling) CheckCombatTarget();
	else CheckPatrolTarget();
	
}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::CheckCombatTarget()
{
	if (!InTargetRange(CombatTarget, CombatRadius))
	{
		// Out of combat radius -> lose interest
		CombatTarget = nullptr;

		if (HealthBarWidgetComponent) HealthBarWidgetComponent->SetVisibility(false);

		EnemyState = EEnemyState::EES_Patrolling;
		GetCharacterMovement()->MaxWalkSpeed = Attributes->GetWalkSpeed();
		MoveToTarget(PatrolTarget);
	}
	else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
	{
		// Outisde attack range -> Chase Character
		EnemyState = EEnemyState::EES_Chasing;
		GetCharacterMovement()->MaxWalkSpeed = Attributes->GetRunSpeed();
		MoveToTarget(CombatTarget);
	}
	else if (InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Attacking)
	{
		// Inside attack range -> Attack Character
		EnemyState = EEnemyState::EES_Attacking;
		/* TODO: Attack montage */
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::Die()
{
	if (HealthBarWidgetComponent) HealthBarWidgetComponent->SetVisibility(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(120.f);

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		FName SectionName = FName();

		switch (int32 Selection = FMath::RandRange(0, 3))
		{
		case 0:
			SectionName = FName("Death1");
			DeathPose = EDeathPose::EDP_Death1;
			break;
		case 1:
			SectionName = FName("Death2");
			DeathPose = EDeathPose::EDP_Death2;
			break;
		case 2:
			SectionName = FName("Death3");
			DeathPose = EDeathPose::EDP_Death3;
			break;
		case 3:
			SectionName = FName("Death4");
			DeathPose = EDeathPose::EDP_Death4;
			break;
		default:
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}
}

bool AEnemy::InTargetRange(AActor* Target, double Radius) const
{
	if (!Target) return false;

	return (Target->GetActorLocation() - GetActorLocation()).Size() <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (!EnemyController || !PatrolTarget) return;
	
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(15.f);
	EnemyController->MoveTo(MoveRequest);

}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	if (Target != PatrolTarget) ValidTargets.AddUnique(Target);

	if (ValidTargets.Num() > 0)
		return ValidTargets[FMath::RandRange(0, ValidTargets.Num() - 1)];
	
	return nullptr;
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	if (EnemyState == EEnemyState::EES_Chasing) return;

	if (SeenPawn->ActorHasTag(FName("SlashCharacter")))
	{
		GetWorldTimerManager().ClearTimer(PatrolTimer);
		GetCharacterMovement()->MaxWalkSpeed = Attributes->GetRunSpeed();
		CombatTarget = SeenPawn;

		if (EnemyState != EEnemyState::EES_Attacking)
		{
			EnemyState = EEnemyState::EES_Chasing;
			MoveToTarget(CombatTarget);
		}
	}
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	if (HealthBarWidgetComponent) HealthBarWidgetComponent->SetVisibility(true);

	if (Attributes && Attributes->IsAlive()) DirectionalHitReact(ImpactPoint);
	else Die();

	if (HitSound) UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);

	if (HitParticles) 
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint);

}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	// Lower Impact Point to the Enemy's Actor Location Z
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// Forward * ToHit = |Forward||ToHit| * cos(theta)
	// |Forward| = 1, |ToHit| = 1, so Forward * ToHit = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	// Take the inverse cosine (arc-cosine) of cos(theta) to get theta
	double Theta = FMath::Acos(CosTheta);
	// convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);

	// if Crossproduct points down, Theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0) Theta *= -1.f;

	FName Section("FromBackLarge");
	if (Theta >= -45.f && Theta < 45.f) Section = FName("FromFrontLarge");
	else if (Theta >= -135.f && Theta < -45.f) Section = FName("FromLeftLarge");
	else if (Theta >= 45.f && Theta < 135.f) Section = FName("FromRightLarge");

	PlayHitReactMontage(Section);
	/*
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 5.f, FColor::Blue, 5.f);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 60.f, 5.f, FColor::Red, 5.f);
	UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.f);
	*/
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (Attributes && HealthBarWidgetComponent)
	{
		Attributes->ReceiveDamage(DamageAmount);	 
		HealthBarWidgetComponent->SetHealthPercent(Attributes->GetHealthPercent());
	}

	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = Attributes->GetRunSpeed();
	CombatTarget = EventInstigator->GetPawn();
	MoveToTarget(CombatTarget);

	return DamageAmount;
}

