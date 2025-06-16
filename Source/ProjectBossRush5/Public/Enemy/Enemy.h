// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UAnimMontage;
class UAttributeComponent;
class UHealthBarComponent;
class UPawnSensingComponent;

UCLASS()
class PROJECTBOSSRUSH5_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	void CheckPatrolTarget();
	void CheckCombatTarget();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	void DirectionalHitReact(const FVector& ImpactPoint);
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
							 AController* EventInstigator, AActor* DamageCauser) override;
protected:
	virtual void BeginPlay() override;
	void Die();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);
	
	/**
	 * Play Montage functions
	 */
	void PlayHitReactMontage(const FName& SectionName);
	void PlayDeathMontage(const FName& SectionName);

	UPROPERTY(BlueprintReadOnly)
	EDeathPose m_DeathPose = EDeathPose::EDP_Alive;

private:
	/**
	 * Components
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* m_Attributes;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* m_HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* m_PawnSensing;
	
	/** 
	* Animation montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* m_HitReactMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* m_DeathMontage;
	
	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* m_HitSound;

	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* m_HitParticles;

	UPROPERTY()
	AActor* m_CombatTarget;

	UPROPERTY(EditAnywhere)
	double m_CombatRadius = 500.0f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	/**
	 * Navigation
	 */
	UPROPERTY()
	class AAIController* m_EnemyController;
	
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* m_PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> m_PatrolTargets;

	UPROPERTY(EditAnywhere)
	double m_PatrolRadius = 200.0f;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMax = 10.f;

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
};
