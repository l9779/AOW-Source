#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GroomComponent.h"

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
	}
}

void ASlashCharacter::Movement(const FInputActionValue& Value)
{
	const FVector2D InputDirection = Value.Get<FVector2D>();

	if (GetController())
	{
		const FRotationMatrix RotationMatrix = FRotationMatrix(FRotator(0.f, GetControlRotation().Yaw, 0.f));
		
		//ForwardDirection where the player is looking = RotationMatrix.GetUnitAxis(EAxis::X);
		AddMovementInput(RotationMatrix.GetUnitAxis(EAxis::X), InputDirection.Y);
		//RightDirection where the player is looking = RotationMatrix.GetUnitAxis(EAxis::Y);
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
