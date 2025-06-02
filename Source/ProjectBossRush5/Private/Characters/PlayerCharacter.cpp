// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	m_CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	m_CameraBoom->SetupAttachment(GetRootComponent());
	m_CameraBoom->TargetArmLength = 300.f;

	m_ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	m_ViewCamera->SetupAttachment(m_CameraBoom);
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &APlayerCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &APlayerCharacter::LookUp);
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &APlayerCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &APlayerCharacter::Attack);
}

void APlayerCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (m_EquippedWeapon && m_EquippedWeapon->GetWeaponBox())
	{
		m_EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		m_EquippedWeapon->IgnoreActors.Empty();
	}
}

void APlayerCharacter::MoveForward(float Value)
{
	if (m_ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller && (Value != 0.f))
	{
		// FVector Forward = GetActorForwardVector();
		// AddMovementInput(Forward, Value);
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (m_ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller && (Value != 0.f))
	{
		// FVector Right = GetActorRightVector();
		// AddMovementInput(Right, Value);

		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void APlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void APlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void APlayerCharacter::EKeyPressed()
{
	AWeapon* OverlappingWeapon = Cast<AWeapon>(m_OverlappingItem);
	if (OverlappingWeapon)
	{
		OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);

		m_CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
		m_OverlappingItem = nullptr;
		m_EquippedWeapon = OverlappingWeapon;
	}
	else
	{
		if (CanDisarm())
		{
			PlayEquipMontage(FName("Unequip"));
			m_CharacterState = ECharacterState::ECS_Unequipped;
			m_ActionState = EActionState::EAS_EquippingWeapon;
		}
		else if (CanArm())
		{
			PlayEquipMontage(FName("Equip"));
			m_CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			m_ActionState = EActionState::EAS_EquippingWeapon;
		}
	}
}

void APlayerCharacter::Attack()
{
	if (CanAtttack())
	{
		PlayAttackMontage();
		m_ActionState = EActionState::EAS_Attacking;
	}
}

void APlayerCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && m_AttackMontage)
	{
		AnimInstance->Montage_Play(m_AttackMontage);
		const int32 Selection = 1;
		FName SectionName = FName();
		switch (Selection)
		{
		case 0:
			SectionName = FName("Attack1");
			break;
		case 1:
			SectionName = FName("Attack2");
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, m_AttackMontage);
	}
}

void APlayerCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && m_EquipMontage)
	{
		AnimInstance->Montage_Play(m_EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, m_EquipMontage);
	}
}

void APlayerCharacter::AttackEnd()
{
	m_ActionState = EActionState::EAS_Unoccupied;
}

bool APlayerCharacter::CanAtttack()
{
	return m_ActionState == EActionState::EAS_Unoccupied && m_CharacterState != ECharacterState::ECS_Unequipped;
}



bool APlayerCharacter::CanDisarm()
{
	return m_ActionState == EActionState::EAS_Unoccupied && 
		m_CharacterState != ECharacterState::ECS_Unequipped;
}

bool APlayerCharacter::CanArm()
{
	return m_ActionState == EActionState::EAS_Unoccupied &&
		m_CharacterState == ECharacterState::ECS_Unequipped && 
		m_EquippedWeapon;
}

void APlayerCharacter::Disarm()
{
	if (m_EquippedWeapon)
	{
		m_EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void APlayerCharacter::Arm()
{
	if (m_EquippedWeapon)
	{
		m_EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void APlayerCharacter::FinishEquipping()
{
	m_ActionState = EActionState::EAS_Unoccupied;
}