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

protected:
	virtual void BeginPlay() override;

	virtual void Attack();
	virtual void Die();
	virtual void DirectionalHitReact(const FVector& ImpactPoint);
	virtual void HandleDamage(float DamageAmount);
	virtual void PlayHitSound(const FVector& ImpactPoint);
	virtual void SpawnHitParticle(const FVector& ImpactPoint);
	void DisableCapsule();
	virtual bool CanAttack();
	virtual bool IsAlive();
	virtual void PlayHitReactMontage(const FName& SectionName);
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	
	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();
	
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
	
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* m_EquippedWeapon;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* m_Attributes;

private:
	virtual void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);

	UPROPERTY(EditAnywhere, Category = Combat)
	USoundBase* m_HitSound;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	UParticleSystem* m_HitParticles;
	
	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* m_AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* m_HitReactMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* m_DeathMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> m_AttackMontageSections;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> m_DeathMontageSections;
};
