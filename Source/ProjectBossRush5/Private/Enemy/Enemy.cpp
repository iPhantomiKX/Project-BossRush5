// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"
#include "AIController.h"
#include "NiagaraCommon.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "HUD/HealthBarComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Kismet/KismetSystemLibrary.h"

#include "ProjectBossRush5/DebugMacros.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	m_HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	m_HealthBarWidget->SetupAttachment(GetRootComponent());

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	m_PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	m_PawnSensing->SightRadius = 4000.f;
	m_PawnSensing->SetPeripheralVisionAngle(45.f);
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(m_PatrolTarget);
}

void AEnemy::HideHealthBar()
{
	if (m_HealthBarWidget)
	{
		m_HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::ShowHealthBar()
{
	if (m_HealthBarWidget)
	{
		m_HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::LoseInterest()
{
	m_CombatTarget = nullptr;
	HideHealthBar();
}

void AEnemy::StartPatrolling()
{
	m_EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = m_PatrollingSpeed;
	MoveToTarget(m_PatrolTarget);
}

void AEnemy::ChaseTarget()
{
	m_EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = m_ChasingSpeed;
	MoveToTarget(m_CombatTarget);
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(m_CombatTarget, m_CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(m_CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing()
{
	return m_EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsEngaged()
{
	return m_EnemyState == EEnemyState::EES_Engaged;
}

bool AEnemy::IsAttacking()
{
	return m_EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead()
{
	return m_EnemyState == EEnemyState::EES_Dead;
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::StartAttackTimer()
{
	m_EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(m_AttackMin, m_AttackMax);
	GetWorldTimerManager().SetTimer(m_AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(m_AttackTimer);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (m_HealthBarWidget)
		m_HealthBarWidget->SetVisibility(false);

	m_EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(m_PatrolTarget);
	if (m_PawnSensing)
	{
		m_PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	UWorld* World = GetWorld();
	if (World && m_WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(m_WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		m_EquippedWeapon = DefaultWeapon;
	}
}

int32 AEnemy::PlayDeathMontage()
{
	const int32 Selection = Super::PlayDeathMontage();
	TEnumAsByte<EDeathPose> Pose(Selection);
	if (Pose < EDeathPose::EDP_MAX)
	{
		m_DeathPose = Pose;
	}

	return Selection;
}

void AEnemy::Die()
{
	m_EnemyState = EEnemyState::EES_Dead;
	PlayDeathMontage();
	ClearAttackTimer();
	HideHealthBar();
	DisableCapsule();
	SetLifeSpan(m_DeathLifeSpan);
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (m_EnemyController == nullptr || Target == nullptr) return;
	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(50.f);
	m_EnemyController->MoveTo(MoveRequest);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : m_PatrolTargets)
	{
		if (Target != m_PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}
	return nullptr;
}

void AEnemy::Attack()
{
	Super::Attack();
	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	bool bCanAttack =
		!IsOutsideAttackRadius() &&
		!IsAttacking() &&
		!IsDead();
	return bCanAttack;
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);

	if (m_Attributes && m_HealthBarWidget)
	{
		m_HealthBarWidget->SetHealthPercent(m_Attributes->GetHealthPercent());
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsDead()) return;
	if (m_EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(m_PatrolTarget, m_PatrolRadius))
	{
		m_PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(WaitMin, WaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();
		LoseInterest();
		if (!IsEngaged()) StartPatrolling();
	}
	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();
		if (!IsEngaged()) ChaseTarget();
	}
	else if (CanAttack())
	{
		StartAttackTimer();
	}
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		m_EnemyState != EEnemyState::EES_Dead &&
		m_EnemyState != EEnemyState::EES_Chasing &&
		m_EnemyState < EEnemyState::EES_Attacking &&
		SeenPawn->ActorHasTag(FName("Player"));
			
	if (bShouldChaseTarget)
	{
		m_CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	ShowHealthBar();
	if (IsAlive())
	{
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		Die();
	}
	PlayHitSound(ImpactPoint);
	SpawnHitParticle(ImpactPoint);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
						 AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	m_CombatTarget = EventInstigator->GetPawn();
	ChaseTarget();
	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if (m_EquippedWeapon)
	{
		m_EquippedWeapon->Destroy();
	}
}



