// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Item.h"

#include "ProjectBossRush5/DebugMacros.h"
#include "Components/SphereComponent.h"
#include "Characters/PlayerCharacter.h"
#include "NiagaraComponent.h"

AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;

	m_ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	m_ItemMesh->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	m_ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = m_ItemMesh;

	m_Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	m_Sphere->SetupAttachment(GetRootComponent());

	m_EmbersEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("EmbersEffect"));
	m_EmbersEffect->SetupAttachment(GetRootComponent());
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	m_Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	m_Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
	
	// UE_LOG(LogTemp, Warning, TEXT("Begin Play called!"));
	// if (GEngine)
	// 	GEngine->AddOnScreenDebugMessage(1, 60.0f, FColor::Red, FString("Item OnScreen Message!"));
	//FVector Location = GetActorLocation();
	//FVector Forward = GetActorForwardVector();

	//DRAW_SPHERE(Location);
	//DRAW_LINE(Location, Location + Forward * 100.0f);
	//DRAW_POINT(Location + Forward * 100.0f);
	//DRAW_VECTOR(Location, Location + Forward * 100.0f);
}

float AItem::TransformedSin()
{
	return m_Amplitude * FMath::Sin(m_RunningTime * m_TimeConstant);
}

float AItem::TransformedCos()
{
	return m_Amplitude * FMath::Cos(m_RunningTime * m_TimeConstant);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		PlayerCharacter->SetOverlappingItem(this);
	}
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(1, 30.f, FColor::Red, OtherActor->GetName());
	// }
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(OtherActor);
	if (PlayerCharacter)
	{
		PlayerCharacter->SetOverlappingItem(nullptr);
	}
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

	// // Movement rate in Units of cm/s
	// float MovementRate = 50.f;
	// float RotationRate = 45.0f;
	//
	// // MovementRate * DeltaTime (cm/s) * (s/frame) = (cm/frame)
	// AddActorWorldOffset(FVector(MovementRate * DeltaTime, 0.f, 0.f));
	// AddActorWorldRotation(FRotator(0.f, RotationRate * DeltaTime, 0.f));

	m_RunningTime += DeltaTime;
	// float DeltaZ = m_Amplitude * FMath::Sin(m_RunningTime * m_TimeConstant);
	// AddActorWorldOffset(FVector(0.f, 0.f, DeltaZ));
	
	// DRAW_SPHERE_SINGLEFRAME(GetActorLocation());
	// DRAW_VECTOR_SINGLEFRAME(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 100.0f);
	//
	// FVector AvgVector = Avg<FVector>(GetActorLocation(), FVector::ZeroVector);
	// DRAW_POINT_SINGLEFRAME(AvgVector);

	if (m_ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}
}

