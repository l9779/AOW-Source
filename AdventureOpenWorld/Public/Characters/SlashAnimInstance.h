#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "CharacterTypes.h"
#include "SlashAnimInstance.generated.h"

UCLASS()
class ADVENTUREOPENWORLD_API USlashAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	//virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class ASlashCharacter> SlashCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UCharacterMovementComponent> SlashCharacterMovement;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	FVector Velocity;

	UPROPERTY(BlueprintReadWrite, Category = "Movement")
	float GroundSpeed;
	UPROPERTY(BlueprintReadWrite, Category = "Movement|Bow")
	float YawOffset;
	UPROPERTY(BlueprintReadWrite, Category = "Movement|Bow")
	float RootYawOffset;
	UPROPERTY(BlueprintReadWrite, Category = "Movement|Bow")
	float DistanceCurve;

	UPROPERTY(BlueprintReadWrite, Category = "Movement|Bow")
	FRotator BaseAimRotation;
	UPROPERTY(BlueprintReadWrite, Category = "Movement|Bow")
	FRotator MovingRotation;

	UPROPERTY(BlueprintReadWrite, Category = "Movement|Bools")
	bool isFalling;
	UPROPERTY(BlueprintReadWrite, Category = "Movement|Bools")
	bool bShouldMove;
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool IsAiming;

	UPROPERTY(BlueprintReadWrite, Category = "Character States")
	ECharacterState CharacterState;
	UPROPERTY(BlueprintReadWrite, Category = "Character States")
	EActionState ActionState;
	UPROPERTY(BlueprintReadWrite, Category = "Character States")
	TEnumAsByte<EDeathPose> DeathPose;
};
