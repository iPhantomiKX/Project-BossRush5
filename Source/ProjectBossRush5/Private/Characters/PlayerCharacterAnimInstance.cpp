// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerCharacterAnimInstance.h"
#include "Characters/PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UPlayerCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	m_PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	if (m_PlayerCharacter)
	{
		m_PlayerCharacterMovement = m_PlayerCharacter->GetCharacterMovement();
	}
}

void UPlayerCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (m_PlayerCharacterMovement)
	{
		m_GroundSpeed = UKismetMathLibrary::VSizeXY(m_PlayerCharacterMovement->Velocity);
		m_IsFalling = m_PlayerCharacterMovement->IsFalling();
		m_CharacterState = m_PlayerCharacter->GetCharacterState();
		ActionState = m_PlayerCharacter->GetActionState();
		DeathPose = m_PlayerCharacter->GetDeathPose();
	}
}
