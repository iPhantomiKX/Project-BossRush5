// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "HUD/PlayerHUD.h"
#include "HUD/PlayerOverlay.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	
	m_CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	m_CameraBoom->SetupAttachment(GetRootComponent());
	m_CameraBoom->TargetArmLength = 300.f;

	m_ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	m_ViewCamera->SetupAttachment(m_CameraBoom);
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &APlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &APlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &APlayerCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &APlayerCharacter::LookUp);
	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &APlayerCharacter::Jump);
	PlayerInputComponent->BindAction(FName("Equip"), IE_Pressed, this, &APlayerCharacter::EKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &APlayerCharacter::Attack);
}

void APlayerCharacter::Jump()
{
	if (IsUnoccupied())
	{
		Super::Jump();
	}
}

float APlayerCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	SetHUDHealth();
	return DamageAmount; 
}

void APlayerCharacter::GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit_Implementation(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
	if (m_Attributes && m_Attributes->GetHealthPercent() > 0.f)
	{
		m_ActionState = EActionState::EAS_HitReaction;
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	Tags.Add("EngageableTarget");
	InitializePlayerOverlay();
}

void APlayerCharacter::MoveForward(float Value)
{
	if (m_ActionState != EActionState::EAS_Unoccupied) return;
	if (Controller && (Value != 0.f))
	{
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
			Disarm();
		}
		else if (CanArm())
		{
			Arm();
		}
	}
}

void APlayerCharacter::Attack()
{
	Super::Attack();
	if (CanAttack())
	{
		PlayAttackMontage();
		m_ActionState = EActionState::EAS_Attacking;
	}
}

void APlayerCharacter::EquipWeapon(AWeapon* Weapon)
{
	Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	m_CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	m_OverlappingItem = nullptr;
	m_EquippedWeapon = Weapon;
}

void APlayerCharacter::AttackEnd()
{
	m_ActionState = EActionState::EAS_Unoccupied;
}

bool APlayerCharacter::CanAttack()
{
	return m_ActionState == EActionState::EAS_Unoccupied
						 && m_CharacterState != ECharacterState::ECS_Unequipped;
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
	PlayEquipMontage(FName("Unequip"));
	m_CharacterState = ECharacterState::ECS_Unequipped;
	m_ActionState = EActionState::EAS_EquippingWeapon;
}

void APlayerCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	m_CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	m_ActionState = EActionState::EAS_EquippingWeapon;
}

void APlayerCharacter::AttachWeaponToBack()
{
	if (m_EquippedWeapon)
	{
		m_EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void APlayerCharacter::AttachWeaponToHand()
{
	if (m_EquippedWeapon)
	{
		m_EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
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

void APlayerCharacter::Die()
{
	Super::Die();

	m_ActionState = EActionState::EAS_Dead;
	DisableMeshCollision();
}

void APlayerCharacter::FinishEquipping()
{
	m_ActionState = EActionState::EAS_Unoccupied;
}

void APlayerCharacter::HitReactEnd()
{
	m_ActionState = EActionState::EAS_Unoccupied;
}

bool APlayerCharacter::IsUnoccupied()
{
	return m_ActionState == EActionState::EAS_Unoccupied;
}

void APlayerCharacter::InitializePlayerOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		APlayerHUD* PlayerHUD = Cast<APlayerHUD>(PlayerController->GetHUD());
		if (PlayerHUD)
		{
			m_PlayerOverlay = PlayerHUD->GetPlayerOverlay();
			if (m_PlayerOverlay && m_Attributes)
			{
				m_PlayerOverlay->SetHealthBarPercent(m_Attributes->GetHealthPercent());
				m_PlayerOverlay->SetStaminaBarPercent(1.f);
			}
		}
	}
}

void APlayerCharacter::SetHUDHealth()
{
	if (m_PlayerOverlay && m_Attributes)
	{
		m_PlayerOverlay->SetHealthBarPercent(m_Attributes->GetHealthPercent());
	}
}
