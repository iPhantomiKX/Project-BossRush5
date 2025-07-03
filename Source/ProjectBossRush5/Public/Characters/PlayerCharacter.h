// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;

UCLASS()
class PROJECTBOSSRUSH5_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	

protected:
	virtual void BeginPlay() override;
	
	/**
	 * Callback for Input
	 */
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EKeyPressed();
	virtual void Attack() override;

	virtual void AttackEnd() override;
	virtual bool CanAttack() override;

	void PlayEquipMontage(const FName& SectionName);
	bool CanDisarm();
	bool CanArm();

	UFUNCTION(BlueprintCallable)
	void Disarm();

	UFUNCTION(BlueprintCallable)
	void Arm();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

private:
	ECharacterState m_CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState m_ActionState = EActionState::EAS_Unoccupied;
	
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* m_CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* m_ViewCamera;

	UPROPERTY(VisibleInstanceOnly)
	AItem* m_OverlappingItem;



	/** 
	* Animation montages
	*/


	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* m_EquipMontage;
public:
	FORCEINLINE void SetOverlappingItem(AItem* item) {m_OverlappingItem = item;}
	FORCEINLINE ECharacterState GetCharacterState() const { return m_CharacterState; }
};
