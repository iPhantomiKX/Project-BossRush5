// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterTypes.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;

UCLASS()
class PROJECTBOSSRUSH5_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EKeyPressed();
private:
	ECharacterState m_CharacterState = ECharacterState::ECS_Unequipped;
	
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* m_CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* m_ViewCamera;

	UPROPERTY(VisibleInstanceOnly)
	AItem* m_OverlappingItem;
public:
	FORCEINLINE void SetOverlappingItem(AItem* item) {m_OverlappingItem = item;}
	FORCEINLINE ECharacterState GetCharacterState() const { return m_CharacterState; }
};
