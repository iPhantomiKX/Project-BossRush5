// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class UPawnSensingComponent;

UCLASS()
class PROJECTBOSSRUSH5_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
							 AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/** </AActor> */

	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </IHitInterface> */
	
protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/** <ABaseCharacter> */
	virtual void Die() override;
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void AttackEnd() override;
	virtual void HandleDamage(float DamageAmount) override;
	/** </ABaseCharacter> */
	
	UPROPERTY(VisibleAnywhere, Category = Combat)
	EEnemyState m_EnemyState = EEnemyState::EES_Patrolling;

private:

	/** AI BEHAVIOUR */
	void InitializeEnemy();
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void PatrolTimerFinished();
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	void ClearPatrolTimer();
	void StartAttackTimer();
	void ClearAttackTimer();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	void SpawnDefaultWeapon();
	
	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);
	
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* m_HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* m_PawnSensing;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> m_WeaponClass;
	
	UPROPERTY(EditAnywhere)
	double m_CombatRadius = 1000.0f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 200.f;
	
	UPROPERTY()
	class AAIController* m_EnemyController;
	
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* m_PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> m_PatrolTargets;

	UPROPERTY(EditAnywhere)
	double m_PatrolRadius = 200.0f;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float m_PatrollingSpeed = 125.0f;
	
	FTimerHandle m_AttackTimer;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float m_AttackMin = 0.1f;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float m_AttackMax = 0.3f;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float m_ChasingSpeed = 300.0f;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float m_DeathLifeSpan = 8.0f;
};
