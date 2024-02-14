#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Items/Weapons/MeeleeWeapon.h"
#include "Components/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseCharacter::ABaseCharacter():
	WarpTargetDistance(75.f)
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (IsAlive() && Hitter)
		DirectionalHitReact(Hitter->GetActorLocation());
	else
		Die();

	ANCB_SetWeaponBoxCollisionEnabled(ECollisionEnabled::NoCollision);
	PlayHitSound(ImpactPoint);
	SpawnHitParticles(ImpactPoint);
}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead"))) CombatTarget = nullptr;
}

void ABaseCharacter::Die_Implementation()
{
	Tags.Add(FName("Dead"));
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PlayDeathMontage();
}

void ABaseCharacter::DirectionalHitReact(const FVector& ImpactPoint)
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

	FName Section("FromBack");
	if (Theta >= -45.f && Theta < 45.f) Section = FName("FromFront");
	else if (Theta >= -135.f && Theta < -45.f) Section = FName("FromLeft");
	else if (Theta >= 45.f && Theta < 135.f) Section = FName("FromRight");

	PlayHitReactMontage(Section);
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound) UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
}

void ABaseCharacter::SpawnHitParticles(const FVector& ImpactPoint)
{
	if (HitParticles)
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, ImpactPoint);
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes) Attributes->ReceiveDamage(DamageAmount);
}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;

	const int32 Selection = FMath::RandRange(0, SectionNames.Num() - 1);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}

int32 ABaseCharacter::PlayAttackMontage(bool bIsHeavyAttack)
{
	UAnimMontage* MontageToPlay;

	switch (EquippedWeapon->GetWeaponType())
	{
	case (EWeaponType::EWT_Default):
		MontageToPlay = DefaultAttackMontage;
		break;
	case (EWeaponType::EWT_OneHandSword):
		MontageToPlay = SwordAttackMontage;
		break;
	case (EWeaponType::EWT_GreatSword):
		MontageToPlay = GreatSwordAttackMontage;
		break;
	default:
		MontageToPlay = DefaultAttackMontage;
		break;
	}

	return PlayRandomMontageSection(MontageToPlay, AttackMontageSections);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection = PlayRandomMontageSection(DeathMontage, DeathMontageSections);

	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_MAX) DeathPose = Pose;

	return Selection;
}

void ABaseCharacter::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

void ABaseCharacter::PlayDodgeMontage()
{
	if (DodgeMontage) PlayMontageSection(DodgeMontage, FName("Default"));
}

void ABaseCharacter::StopAttackMontage()
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		UAnimMontage* CurrentPlayingMontage = AnimInstance->GetCurrentActiveMontage();
		AnimInstance->Montage_Stop(.25f, CurrentPlayingMontage);
	}
}

/* < Called on BP_Enemy Tick > */
FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (!CombatTarget) return FVector();

	const FVector& CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector& Location = GetActorLocation();

	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToMe *= WarpTargetDistance;

	return CombatTargetLocation + TargetToMe;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget) return CombatTarget->GetActorLocation();

	return FVector();
}
/* < /Called on BP_Enemy Tick > */

bool ABaseCharacter::CanAttack() const
{
	return false;
}

bool ABaseCharacter::IsAlive() const
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::ANCB_SetWeaponBoxCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (AMeeleeWeapon* EquippedMeeleeWeapon = Cast<AMeeleeWeapon>(EquippedWeapon))
	{
		if (EquippedMeeleeWeapon->GetWeaponBox())
		{
			EquippedMeeleeWeapon->ClearIgnoreActors();
			EquippedMeeleeWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		}
	}
}

void ABaseCharacter::ANCB_AttackEnd()
{
}

void ABaseCharacter::ANCB_DodgeEnd()
{
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
