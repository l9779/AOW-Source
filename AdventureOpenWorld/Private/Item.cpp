#include "Item.h"
#include "AdventureOpenWorld/DebugMacros.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	DRAW_SPHERE(GetActorLocation(), FColor::Blue)
	SHOW_NAME
		
	DRAW_VECTOR(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.f, FColor::Yellow)
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FString Message = FString::Printf(TEXT("Delta Time: %f"), DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
}

