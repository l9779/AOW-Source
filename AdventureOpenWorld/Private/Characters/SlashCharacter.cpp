#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GroomComponent.h"
#include "Actors/Item.h"
#include "Actors/Weapons/Weapon.h"
#include "Animation/AnimInstance.h"

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
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(SlashMappingContext, 0);
		}
	}
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Movement);
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &ASlashCharacter::LookAround);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started , this, &ASlashCharacter::Jump);
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ASlashCharacter::EKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ASlashCharacter::Attack);
	}
}

void ASlashCharacter::Movement(const FInputActionValue& Value)
{
	if (ActionState == EActionState::EAS_Attacking) return;

	const FVector2D InputDirection = Value.Get<FVector2D>();

	if (GetController())
	{
		const FRotationMatrix RotationMatrix = FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f));
		
		//Forward Vector where the player is looking in game world = RotationMatrix.GetUnitAxis(EAxis::X);
		AddMovementInput(RotationMatrix.GetUnitAxis(EAxis::X), InputDirection.Y);
		//Right Vector from where the player is looking in game world = RotationMatrix.GetUnitAxis(EAxis::Y);
		AddMovementInput(RotationMatrix.GetUnitAxis(EAxis::Y), InputDirection.X);
	}
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
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"));
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		OverlappingItem = nullptr;
		EquippedWeapon = OverlappingWeapon;
	} 
	else if (CanDisarm())
	{
		PlayEquipMontage(FName("Unequip"));
		CharacterState = ECharacterState::ECS_Unequipped;
	}
	else if (CanArm())
	{
		PlayEquipMontage(FName("Equip"));
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
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
		
	if (AttackMontage && AnimInstance)
	{
		AnimInstance->Montage_Play(AttackMontage);

		FName SectionName = FName();
		
		switch (int32 Selection = FMath::RandRange(0, 1))
		{
		case (0):
			SectionName = FName("Attack1");
			break;
		case (1):
			SectionName = FName("Attack2");
			break;
		default:
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

void ASlashCharacter::PlayEquipMontage(FName SectionName)
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