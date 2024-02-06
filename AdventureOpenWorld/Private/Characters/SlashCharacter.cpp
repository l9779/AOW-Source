#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GroomComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"

ASlashCharacter::ASlashCharacter():
	CharacterState(ECharacterState::ECS_Unequipped),
	ActionState(EActionState::EAS_Unoccupied)
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;
	SpringArm->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	HeadHair = CreateDefaultSubobject<UGroomComponent>(TEXT("HeadHair"));
	HeadHair->SetupAttachment(GetMesh());
	HeadHair->AttachmentName = FString("head");

	EyebrownHair = CreateDefaultSubobject<UGroomComponent>(TEXT("EyebrownHair"));
	EyebrownHair->SetupAttachment(GetMesh());
	EyebrownHair->AttachmentName = FString("head");

}

void ASlashCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		InitializeSlashOverlay(PlayerController);

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			Subsystem->AddMappingContext(SlashMappingContext, 0);
	}

	Tags.Add(FName("EngageableTarget"));

	if (Attributes) GetCharacterMovement()->MaxWalkSpeed = Attributes->GetRunSpeed();

}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (OrientAttackToRotation) OrientAttackRotation(DeltaTime);
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Movement);
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::None , this, &ASlashCharacter::ClearMovement);
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &ASlashCharacter::LookAround);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started , this, &ASlashCharacter::Jump);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ASlashCharacter::EKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ASlashCharacter::Attack);
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Started, this, &ASlashCharacter::Walk);
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &ASlashCharacter::LeftShiftPressed);
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::None, this, &ASlashCharacter::LeftShiftReleased);
	}
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	return DamageAmount;
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	ActionState = EActionState::EAS_HitReaction;
}

void ASlashCharacter::Movement(const FInputActionValue& Value)
{
	if (ActionState == EActionState::EAS_Attacking) return;

	const FVector2D InputDirection = Value.Get<FVector2D>();

	InputY = InputDirection.Y;
	InputX = InputDirection.X;

	if (GetController())
	{
		const FRotationMatrix RotationMatrix = FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f));
		
		//Forward Vector where the player is looking in game world = RotationMatrix.GetUnitAxis(EAxis::X);
		AddMovementInput(RotationMatrix.GetUnitAxis(EAxis::X), InputDirection.Y);
		//Right Vector from where the player is looking in game world = RotationMatrix.GetUnitAxis(EAxis::Y);
		AddMovementInput(RotationMatrix.GetUnitAxis(EAxis::Y), InputDirection.X);
	}
}

void ASlashCharacter::ClearMovement(const FInputActionValue& Value)
{
	const FVector2D InputDirection = Value.Get<FVector2D>();

	if (InputDirection.Y == 0.f) InputY = InputDirection.Y;
	if (InputDirection.X == 0.f) InputX = InputDirection.X;
}

void ASlashCharacter::LookAround(const FInputActionValue& Value)
{
	const FVector2D LookDirectionAxis = Value.Get<FVector2D>();

	if (GetController())
	{
		AddControllerPitchInput(LookDirectionAxis.Y);
		AddControllerYawInput(LookDirectionAxis.X);
	}
}

void ASlashCharacter::Walk()
{
	if (WalkMode)
		GetCharacterMovement()->MaxWalkSpeed = Attributes->GetRunSpeed();
	else 
		GetCharacterMovement()->MaxWalkSpeed = Attributes->GetWalkSpeed();

	WalkMode = !WalkMode;
}

void ASlashCharacter::EKeyPressed()
{
	if (AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem))
		EquipWeapon(OverlappingWeapon);
	else if (CanDisarm())
		SheatWeapon();
	else if (CanArm())
		UnsheatWeapon();
}

void ASlashCharacter::Attack()
{
	if (CanAttack())
	{
		PlayAttackMontage(HoldingHeavyAttack);
		ActionState = EActionState::EAS_Attacking;
	} 
	else if (CanArm())
	{
		UnsheatWeapon();
	}
}

void ASlashCharacter::LeftShiftPressed()
{
	HoldingHeavyAttack = true;
}

void ASlashCharacter::LeftShiftReleased()
{
	HoldingHeavyAttack = false;
}

bool ASlashCharacter::CanAttack() const
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

void ASlashCharacter::OrientAttackRotation(float DeltaTime)
{
	FRotator AttackRotation(FRotator::ZeroRotator);

	if (InputX != 0.f || InputY != 0.f)
	{
		FVector InputDirection(InputY, InputX, 0.f);
		FRotator InputRotator = UKismetMathLibrary::MakeRotationFromAxes(InputDirection, FVector::ZeroVector, FVector::ZeroVector);
		FRotator DirectionRotator = UKismetMathLibrary::ComposeRotators(InputRotator, GetControlRotation());
		AttackRotation = FRotator(0.f, DirectionRotator.Yaw, 0.f);
	}
	else
	{
		AttackRotation = GetActorRotation();
		AttackRotation.Yaw = GetControlRotation().Yaw;
	}

	SetActorRotation(FMath::RInterpTo(GetActorRotation(), AttackRotation, DeltaTime, 10.f));
}

void ASlashCharacter::UnsheatWeapon()
{
	FName SectionName("Equip");
	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_GreatSword)
		SectionName = FName("EquipGreatSword");

	PlayMontageSection(EquipMontage, SectionName);
	ActionState = EActionState::EAS_EquippingWeapon;
	SetCharacterStateOnWeapon();
}

void ASlashCharacter::SheatWeapon()
{
	FName SectionName("Unequip");
	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_GreatSword)
		SectionName = FName("UnequipGreatSword");

	PlayMontageSection(EquipMontage, SectionName);
	ActionState = EActionState::EAS_EquippingWeapon;
	CharacterState = ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanDisarm() const
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ASlashCharacter::CanArm() const
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		EquippedWeapon;
}

void ASlashCharacter::EquipWeapon(AWeapon* Weapon)
{
	if (EquippedWeapon) EquippedWeapon->Unequip();

	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	EquippedWeapon = Weapon;
	OverlappingItem = nullptr;

	SetCharacterStateOnWeapon();
}

void ASlashCharacter::ANCB_AttackEnd()
{ 
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::ANCB_AttachWeaponToSheat()
{
	EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::ANCB_AttachWeaponToHand()
{
	EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::ANCB_SetDirectionAttack(bool b)
{
	OrientAttackToRotation = b;
}

void ASlashCharacter::ANCB_HitReactEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::SetCharacterStateOnWeapon()
{
	switch (EquippedWeapon->GetWeaponType())
	{
	case (EWeaponType::EWT_OneHandAxe):
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		break;
	case (EWeaponType::EWT_OneHandSword):
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		break;
	case (EWeaponType::EWT_GreatSword):
		CharacterState = ECharacterState::ECS_EquippedTwoHandedWeapon;
		break;
	default:
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		break;
	}
}

void ASlashCharacter::InitializeSlashOverlay(APlayerController* PlayerController)
{
	if (ASlashHUD* SlashHUD = Cast<ASlashHUD>(PlayerController->GetHUD()))
	{
		SlashOverlay = SlashHUD->GetSlashOverlay();
		if (SlashOverlay && Attributes)
			SlashOverlay->SetAllOverlayValues(0, 0, Attributes->GetHealthPercent());
	}
}
