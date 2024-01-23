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
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<class ASlashCharacter> SlashCharacter;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	TObjectPtr<class UCharacterMovementComponent> SlashCharacterMovement;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float GroundSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	bool isFalling;
	UPROPERTY(BlueprintReadOnly, Category = "Movement|Character State")
	ECharacterState CharacterState;
};
