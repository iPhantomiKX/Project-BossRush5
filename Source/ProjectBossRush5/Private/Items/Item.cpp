// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"
#include "ProjectBossRush5/DebugMacros.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	// UE_LOG(LogTemp, Warning, TEXT("Begin Play called!"));
	//
	// if (GEngine)
	// 	GEngine->AddOnScreenDebugMessage(1, 60.0f, FColor::Red, FString("Item OnScreen Message!"));
	UWorld* World = GetWorld();
	FVector Location = GetActorLocation();
	FVector Forward = GetActorForwardVector();

	DRAW_SPHERE(Location);
	//DRAW_LINE(Location, Location + Forward * 100.0f);
	//DRAW_POINT(Location + Forward * 100.0f);
	DRAW_VECTOR(Location, Location + Forward * 100.0f);
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// UE_LOG(LogTemp, Warning, TEXT("Delta Time: %f"), DeltaTime);
	// if (GEngine)
	// {
	// 	FString Name = GetName();
	// 	FString Message = FString::Printf(TEXT("Item Name: %s"), *Name);
	// 	GEngine->AddOnScreenDebugMessage(1, 60.0f, FColor::Red, Message);
	// 	UE_LOG(LogTemp, Warning, TEXT("Item Name: %s"), *Name);
	// }
}

