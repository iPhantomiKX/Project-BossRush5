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

	virtual void PlayHitReactMontage(const FName& SectionName);
	virtual void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void PlayHitSound(const FVector& ImpactPoint);
	virtual void SpawnHitParticle(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	virtual void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	void DisableCapsule();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	virtual bool CanAttack();
	virtual bool IsAlive();

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

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> m_AttackMontageSections;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> m_DeathMontageSections;
	
	/**
	 * Components
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* m_Attributes;

private:
	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* m_HitSound;

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* m_HitParticles;
};
