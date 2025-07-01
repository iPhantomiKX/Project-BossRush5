// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/HitInterface.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class PROJECTBOSSRUSH5_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
protected:
	virtual void BeginPlay() override;

	virtual void Attack();
	virtual void Die();
	/**
	 * Play Montage functions
	 */
	virtual void PlayAttackMontage();
	virtual void PlayHitReactMontage(const FName& SectionName);
	virtual void DirectionalHitReact(const FVector& ImpactPoint);

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	virtual bool CanAttack();

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* m_EquippedWeapon;

	/** 
	* Animation montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* m_AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* m_HitReactMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* m_DeathMontage;
	
	/**
	 * Components
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* m_Attributes;
	
	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* m_HitSound;

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* m_HitParticles;
};
