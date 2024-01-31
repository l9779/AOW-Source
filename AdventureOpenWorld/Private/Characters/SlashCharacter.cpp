#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimInstance.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"

ASlashCharacter::ASlashCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

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
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			Subsystem->AddMappingContext(SlashMappingContext, 0);

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
	}
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
	if (WalkMode) GetCharacterMovement()->MaxWalkSpeed = JogSpeed;
	else GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	WalkMode = !WalkMode;
}

void ASlashCharacter::EKeyPressed()
{
	if (AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem))
	{
		FName SocketName("OneHandSwordSocket");

		if (OverlappingWeapon->GetEquipState() == ECharacterState::ECS_EquippedTwoHandedWeapon)
			SocketName = FName("GreatSwordSocket");

		if (EquippedWeapon) 
		{
			// Unequip Function
		}

		OverlappingWeapon->Equip(GetMesh(), SocketName, this, this);
		CharacterState = OverlappingWeapon->GetEquipState();
		EquippedWeapon = OverlappingWeapon;
		OverlappingItem = nullptr;
	} 
	else if (CanDisarm())
	{
		FName SectionName("Unequip");

		if (EquippedWeapon->GetEquipState() == ECharacterState::ECS_EquippedTwoHandedWeapon)
			SectionName = FName("UnequipGreatSword");

		PlayEquipMontage(SectionName);
		CharacterState = ECharacterState::ECS_Unequipped;
	}
	else if (CanArm())
	{
		FName SectionName("Equip");

		if (EquippedWeapon->GetEquipState() == ECharacterState::ECS_EquippedTwoHandedWeapon)
			SectionName = FName("EquipGreatSword");

		PlayEquipMontage(SectionName);
		CharacterState = OverlappingWeapon->GetEquipState();
	}
}

void ASlashCharacter::Attack()
{
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

bool ASlashCharacter::CanAttack() const
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
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

void ASlashCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	UAnimMontage* AttackMontage = OneHandSwordAttackMontage;

	if (EquippedWeapon->GetEquipState() == ECharacterState::ECS_EquippedTwoHandedWeapon)
		AttackMontage = GreatSwordAttackMontage;

	if (AttackMontage && AnimInstance)
	{
		AnimInstance->Montage_Play(AttackMontage);

		FName SectionName = FName();
		
		switch (int32 Selection = FMath::RandRange(0, 3))
		{
		case (0):
			SectionName = FName("Attack1");
			break;
		case (1):
			SectionName = FName("Attack2");
			break;
		case (2):
			SectionName = FName("Attack3");
			break;
		case (3):
			SectionName = FName("Attack4");
			break;
		default:
			break;
		}

		// !! Development only !!
		if (AttackMontageOvewrite) SectionName = AttackSectionName;

		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
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

void ASlashCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (EquipMontage && AnimInstance) 
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
		ActionState = EActionState::EAS_EquippingWeapon;
	}
}

void ASlashCharacter::ANCB_AttackEnd()
{ 
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::ANCB_Disarm()
{
	EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::ANCB_Arm()
{
	EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::ANCB_SetWeaponBoxCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->ClearIgnoreActors();
	}
}

void ASlashCharacter::ANCB_SetDirectionAttack(bool b)
{
	OrientAttackToRotation = b;
}
