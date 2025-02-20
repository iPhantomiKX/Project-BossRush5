// Fill out your copyright notice in the Description page of Project Settings.


#include "Pawns/Bird.h"
#include "Components/CapsuleComponent.h"	//Temp
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"

// Sets default values
ABird::ABird()
{
	PrimaryActorTick.bCanEverTick = true;

	m_CapsuleCollider = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	m_CapsuleCollider->SetCapsuleHalfHeight(20.0f);
	m_CapsuleCollider->SetCapsuleRadius(15.0f);
	SetRootComponent(m_CapsuleCollider);

	m_BirdMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BirdMesh"));
	m_BirdMesh->SetupAttachment(GetRootComponent());

	m_CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	m_CameraBoom->SetupAttachment(GetRootComponent());
	m_CameraBoom->TargetArmLength = 300.f;
	
	m_ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	m_ViewCamera->SetupAttachment(m_CameraBoom);

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ABird::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABird::MoveForward(float Value)
{
	if (Controller && (Value != 0.f))
	{
		FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, Value);
	}
}

void ABird::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void ABird::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void ABird::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABird::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ABird::MoveForward);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &ABird::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &ABird::LookUp);
}

