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
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	/** </IHitInterface> */
	
protected:
	virtual void BeginPlay() override;
	
	/** Callbacks for Input */
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void EKeyPressed();
	virtual void Attack() override;

	/** Combat */
	void EquipWeapon(AWeapon* Weapon);
	virtual void AttackEnd() override;
	virtual bool CanAttack() override;
	bool CanDisarm();
	bool CanArm();
	
	UFUNCTION(BlueprintCallable)
	void Disarm();
	UFUNCTION(BlueprintCallable)
	void Arm();
	
	void PlayEquipMontage(const FName& SectionName);

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();
	
	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();
	
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

private:
	/** Character Components */
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* m_CameraBoom;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* m_ViewCamera;
	
	UPROPERTY(VisibleInstanceOnly)
	AItem* m_OverlappingItem;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* m_EquipMontage;
	
	ECharacterState m_CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState m_ActionState = EActionState::EAS_Unoccupied;
	
public:
	FORCEINLINE void SetOverlappingItem(AItem* item) {m_OverlappingItem = item;}
	FORCEINLINE ECharacterState GetCharacterState() const { return m_CharacterState; }
};
