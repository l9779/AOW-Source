#include "Item.h"
#include "AdventureOpenWorld/AdventureOpenWorld.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	FString Name = GetName();
	FString Message = FString::Printf(TEXT("Object Name: %s"), *Name);

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(0, 1.f, FColor::Cyan, Message);

	DRAW_SPHERE(GetActorLocation())
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FString Message = FString::Printf(TEXT("Delta Time: %f"), DeltaTime);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);

}

