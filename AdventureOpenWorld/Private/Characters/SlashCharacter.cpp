#include "Characters/SlashCharacter.h"
#include "Characters/SlashCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GroomComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/InventoryComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Weapons/DistanceWeapon.h"
#include "Items/Pickups/Soul.h"
#include "Items/Pickups/Treasure.h"
//#include "Items/LootableSupplies/LootableSupplies.h"
#include "Interfaces/InteractableInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "HUD/SlashHUD.h"
#include "HUD/SlashOverlay.h"

ASlashCharacter::ASlashCharacter():
	CharacterState(ECharacterState::ECS_Unequipped),
	ActionState(EActionState::EAS_Unoccupied),
	DefaultBoomLength(400.f), DefaultBoomOffset(0.f),
	AimingBoomOffset(0.f, 70.f, 0.f), AimingBoomLength(100.f),
	CameraZoomSpeed(10.f)
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
	SpringArm->TargetArmLength = DefaultBoomLength;
	SpringArm->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	HeadHair = CreateDefaultSubobject<UGroomComponent>(TEXT("HeadHair"));
	HeadHair->SetupAttachment(GetMesh());
	HeadHair->AttachmentName = FString("head");

	EyebrownHair = CreateDefaultSubobject<UGroomComponent>(TEXT("EyebrownHair"));
	EyebrownHair->SetupAttachment(GetMesh());
	EyebrownHair->AttachmentName = FString("head");

	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory Component"));

	PotionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Potion Mesh"));
	PotionMesh->SetupAttachment(GetMesh(), FName("LeftHandSocket"));
	PotionMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PotionMesh->SetVisibility(false);

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

	InitializeCharacterProperties();

	if (SpawnWithWeapon && SpawnWeaponClass && GetWorld())
		if (AWeapon* SpawnedWeapon = GetWorld()->SpawnActor<AWeapon>(SpawnWeaponClass, GetActorTransform()))
			EquipWeapon(SpawnedWeapon);
}

void ASlashCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateStamina(DeltaTime);
	if (OrientAttackToRotation) OrientAttackRotation(DeltaTime);
	UpdateBowTransforms();
	UpdateCamera(DeltaTime);

	if (bHoldingPickup && OverlappingItem) CheckForInteractable();
}

void ASlashCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ASlashCharacter::Movement);
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Completed, this, &ASlashCharacter::ClearMovement);
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &ASlashCharacter::LookAround);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started , this, &ASlashCharacter::Jump);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ASlashCharacter::InteractKeyPressed);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &ASlashCharacter::InteractKeyReleased);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ASlashCharacter::Attack);
		EnhancedInputComponent->BindAction(WalkAction, ETriggerEvent::Started, this, &ASlashCharacter::Walk);
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Started, this, &ASlashCharacter::LeftShiftPressed);
		EnhancedInputComponent->BindAction(HeavyAttackAction, ETriggerEvent::Completed, this, &ASlashCharacter::LeftShiftReleased);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &ASlashCharacter::Dodge);
		EnhancedInputComponent->BindAction(DrinkPotionAction, ETriggerEvent::Started, this, &ASlashCharacter::DrinkPotion);
		EnhancedInputComponent->BindAction(RightMouseAction, ETriggerEvent::Started, this, &ASlashCharacter::RightMousePressed);
		EnhancedInputComponent->BindAction(RightMouseAction, ETriggerEvent::Completed, this, &ASlashCharacter::RightMouseReleased);
		EnhancedInputComponent->BindAction(ArmOrDisarmAction, ETriggerEvent::Started, this, &ASlashCharacter::ArmOnDisarm);
	}
}

void ASlashCharacter::InitializeCharacterProperties()
{
	Tags.Add(FName("EngageableTarget"));
	Tags.Add(FName("PlayerCharacter"));

	if (Attributes) GetCharacterMovement()->MaxWalkSpeed = Attributes->GetRunSpeed();
	SetCameraMode(false);
}

float ASlashCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();

	return DamageAmount;
}

void ASlashCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	if (ActionState == EActionState::EAS_Dodge) Tags.Remove("Dodging");

	Super::GetHit_Implementation(ImpactPoint, Hitter);

	if (Attributes && Attributes->GetHealthPercent() > 0.f) 
		ActionState = EActionState::EAS_HitReaction;
}

void ASlashCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void ASlashCharacter::AddSouls(ASoul* Soul)
{
	if (Attributes)
	{
		Attributes->AddSouls(Soul->GetSouls());
		if (SlashOverlay) SlashOverlay->SetSouls(Attributes->GetSouls());
	}
}

void ASlashCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes)
	{
		Attributes->AddGold(Treasure->GetGold());
		if (SlashOverlay) SlashOverlay->SetGold(Attributes->GetGold());
	}
}

bool ASlashCharacter::CollectHealthPotion()
{
	if (Inventory && Inventory->HasSpaceForHealthPotion())
	{
		Inventory->AddHealthPotion();
		SetHUDPotionCount();
		return true;
	}

	return false;
}

void ASlashCharacter::Movement(const FInputActionValue& Value)
{
	// this can be improoved with a proper game over mechanic (Dettach from controller, etc)
	if (ActionState == EActionState::EAS_Attacking || !IsAlive()) return;

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

void ASlashCharacter::InteractKeyPressed()
{
	if (IInteractableInterface* InteractableInterface = Cast<IInteractableInterface>(OverlappingItem)) // replace this with  GrabableInterface
	{
		InteractableInterface->SetIsBeignActivated(true);
		bHoldingPickup = true;
	}
}

void ASlashCharacter::InteractKeyReleased()
{
	bHoldingPickup = false;

	if (OverlappingItem)
		if (IInteractableInterface* InteractableInterface = Cast<IInteractableInterface>(OverlappingItem))
			InteractableInterface->SetIsBeignActivated(false);
}

void ASlashCharacter::Jump()
{
	if (IsUnoccupied() && !GetCharacterMovement()->IsFalling()) Super::Jump();
}

void ASlashCharacter::Attack()
{
	if (CanAttack())
	{
		if (ADistanceWeapon* BowWeapon = Cast<ADistanceWeapon>(EquippedWeapon))
		{
			if (!CanFireArrow()) return;

			BowWeapon->FireArrow();
			if (FireBowMontage) PlayAnimMontage(FireBowMontage);	
			ActionState = EActionState::EAS_Attacking;
		}
		else
		{
			Attributes->UseStamina(EquippedWeapon->GetStaminaAttackCost());
			SetHUDStamina();

			PlayAttackMontage(HoldingHeavyAttack);
			ActionState = EActionState::EAS_Attacking;
		}
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

void ASlashCharacter::Dodge()
{
	if (IsOccupied() || !HasEnoughStamina()) return;

	Tags.Add(FName("Dodging"));

	PlayDodgeMontage();
	ActionState = EActionState::EAS_Dodge;

	if (Attributes)
	{
		Attributes->UseStamina(Attributes->GetStaminaDodgeCost());
		SetHUDStamina();
	}
}

void ASlashCharacter::DrinkPotion()
{
	if (Attributes)
	{
		if (!Attributes->IsHurt()) return;
	
		if (Inventory && Inventory->GetHealthPotionCount() > 0)
		{
			Inventory->UseHealthPotion();
			Attributes->Heal(Inventory->GetHealthPotionRecovery());
			SetHUDHealth();
			SetHUDPotionCount();

			if (EquipMontage) PlayMontageSection(EquipMontage, FName("DrinkPotion"));
			ActionState = EActionState::EAS_EquippingWeapon;
			ANCB_SetPotionVisibility(true);
		}
	}
}

void ASlashCharacter::RightMousePressed()
{
	if (!CanAttack()) return;

	if (ADistanceWeapon* BowWeapon = Cast<ADistanceWeapon>(EquippedWeapon))
	{
		bAimingBow = true;
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;

		BowWeapon->StringPulled(true);
		BowWeapon->SetIsAimed(true);

		SetCameraMode(true);
		SetCrosshairVisibility(ESlateVisibility::Visible);
	}
	//else if (CharacterState == ECharacterState::ECS_MeeleeWeapon) Block();
}

void ASlashCharacter::RightMouseReleased()
{
	if (bAimingBow)
	{
		bAimingBow = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		bUseControllerRotationYaw = false;

		if (ADistanceWeapon* BowWeapon = Cast<ADistanceWeapon>(EquippedWeapon))
		{
			BowWeapon->SetStringPulled(false); // does not set arrow invisble
			BowWeapon->SetIsAimed(false);
		}

		SetCameraMode(false);
		SetCrosshairVisibility(ESlateVisibility::Hidden);
	}
}

void ASlashCharacter::ArmOnDisarm()
{
	if (CanDisarm()) 
		SheatWeapon();
	else if (CanArm())
		UnsheatWeapon();
}

bool ASlashCharacter::CanAttack() const
{
	return ActionState == EActionState::EAS_Unoccupied &&
	CharacterState != ECharacterState::ECS_Unequipped &&
	HasEnoughStamina(EquippedWeapon->GetStaminaAttackCost());
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

void ASlashCharacter::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (EquippedWeapon) EquippedWeapon->Unequip();

	FName SocketName("RightHandWeaponSocket");
	if (WeaponToEquip->GetWeaponType() == EWeaponType::EWT_Bow)
	SocketName = FName("LeftHandWeaponSocket");

	WeaponToEquip->SetIsBeignActivated(false);
	WeaponToEquip->Equip(GetMesh(), SocketName, this, this);
	EquippedWeapon = WeaponToEquip;
	OverlappingItem = nullptr;

	SetCharacterStateOnWeapon();
}

void ASlashCharacter::Die_Implementation()
{
	Super::Die_Implementation();

	ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

bool ASlashCharacter::HasEnoughStamina(float StaminaCost) const
{
	return Attributes && Attributes->GetStamina() > StaminaCost;
}

bool ASlashCharacter::IsOccupied() const
{
	return ActionState != EActionState::EAS_Unoccupied;
}

void ASlashCharacter::ANCB_AttackEnd()
{ 
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::ANCB_DodgeEnd()
{
	Super::ANCB_DodgeEnd();

	Tags.Remove(FName("Dodging"));
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::ANCB_AttachWeaponToSheat()
{
	EquippedWeapon->SetItemState(EItemState::EIS_Holstered);
	EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::ANCB_AttachWeaponToHand()
{
	FName SocketName("RightHandWeaponSocket");

	if (EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Bow)
		SocketName = FName("LeftHandWeaponSocket");

	EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
	EquippedWeapon->AttachMeshToSocket(GetMesh(), SocketName);
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

void ASlashCharacter::ANCB_SetPotionVisibility(bool Visiblity)
{
	PotionMesh->SetVisibility(Visiblity);

	// if false means is the end of the drinking animation, so set the state to unoccupied
	if (!Visiblity) ActionState = EActionState::EAS_Unoccupied;
}

void ASlashCharacter::ANCB_ReleaseBowString() const
{
	if (ADistanceWeapon* BowWeapon = Cast<ADistanceWeapon>(EquippedWeapon))
		BowWeapon->StringPulled(false);
}

void ASlashCharacter::ANCB_GrabBowString() const
{
	if (ADistanceWeapon* BowWeapon = Cast<ADistanceWeapon>(EquippedWeapon))
		BowWeapon->StringPulled(true);
}

void ASlashCharacter::UpdateStamina(float DeltaTime)
{
	if (Attributes && Attributes->NeedsToRecoverStamina())
	{
		Attributes->RegenStamina(DeltaTime);
		SetHUDStamina();
	}
}

void ASlashCharacter::UpdateBowTransforms()
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponType() == EWeaponType::EWT_Bow)
	{
		ArrowSocketTransform = GetMesh()->GetSocketTransform(FName("RightHandArrowSocket"));
		BowStringTranslation = GetMesh()->GetSocketLocation(FName("RightHandWeaponSocket"));
	}
}

void ASlashCharacter::UpdateCamera(float DeltaTime)
{
	if (CameraNeedsUpdate())
	{
		SpringArm->TargetArmLength =
			FMath::FInterpTo(SpringArm->TargetArmLength, TargetBoomLength, DeltaTime, CameraZoomSpeed);
		SpringArm->SocketOffset =
			FMath::VInterpTo(SpringArm->SocketOffset, TargetBoomOffset, DeltaTime, CameraZoomSpeed);
	}
}

void ASlashCharacter::CheckForInteractable()
{
	IInteractableInterface* InteractableInterface = Cast<IInteractableInterface>(OverlappingItem);

	if (InteractableInterface && InteractableInterface->GetInteractionConcluded())
	{
		if (AWeapon* OverlappingWeapon = Cast<AWeapon>(InteractableInterface))
			EquipWeapon(OverlappingWeapon);
		else 
			InteractableInterface->OnInteractionConcluded();
	}
}

bool ASlashCharacter::IsUnoccupied() const
{
	return ActionState == EActionState::EAS_Unoccupied;
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
	case (EWeaponType::EWT_Bow):
		CharacterState = ECharacterState::ECS_EquippedBow;
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
		{
			SetHUDHealth();
			SetHUDStamina();
			SetHUDPotionCount();
			SetCrosshairVisibility(ESlateVisibility::Hidden);
			SlashOverlay->SetGold(Attributes->GetGold());
			SlashOverlay->SetSouls(Attributes->GetSouls());
		}
	}
}

void ASlashCharacter::SetHUDHealth()
{
	if (SlashOverlay && Attributes)
		SlashOverlay->SetHealthBarPercent(Attributes->GetHealthPercent());
}

void ASlashCharacter::SetHUDStamina()
{
	if (SlashOverlay && Attributes)
		SlashOverlay->SetStaminaBarPercent(Attributes->GetStaminaPercent());
}

void ASlashCharacter::SetCrosshairVisibility(ESlateVisibility SlateVisiblity)
{
	if (SlashOverlay) SlashOverlay->SetCrosshairVisibility(SlateVisiblity);
}

bool ASlashCharacter::CanFireArrow() const
{
	return bAimingBow; // && HasArrowAmmo
}

void ASlashCharacter::SetCameraMode(bool AimingMode)
{
	if (AimingMode)
	{
		TargetBoomLength = AimingBoomLength;
		TargetBoomOffset = AimingBoomOffset;
	}
	else
	{ 
		TargetBoomLength = DefaultBoomLength;
		TargetBoomOffset = DefaultBoomOffset;
	}
}

bool ASlashCharacter::CameraNeedsUpdate() const
{
	return 
		SpringArm->TargetArmLength != TargetBoomLength ||
		SpringArm->TargetOffset != TargetBoomOffset;
}

void ASlashCharacter::SetHUDPotionCount()
{
	if (SlashOverlay && Inventory) 
		SlashOverlay->SetPotionCount(Inventory->GetHealthPotionCount());
}