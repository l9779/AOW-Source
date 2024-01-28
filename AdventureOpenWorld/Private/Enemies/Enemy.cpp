#include "Enemies/Enemy.h"
#include "Components/CapsuleComponent.h"
#include "AdventureOpenWorld/DebugMacros.h"
#include "Animation/AnimInstance.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"

AEnemy::AEnemy()
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

}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	if (HealthBarWidgetComponent) HealthBarWidgetComponent->SetHealthPercent(.5f);

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

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	DirectionalHitReact(ImpactPoint);

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

	if (GEngine) GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Emerald, FString::Printf(TEXT("Theta %f"), Theta));
}

