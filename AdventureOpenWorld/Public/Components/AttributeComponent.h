// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVENTUREOPENWORLD_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
