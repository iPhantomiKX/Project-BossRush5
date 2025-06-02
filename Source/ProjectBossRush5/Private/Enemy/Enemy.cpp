// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Enemy.h"

#include <string>

#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "ProjectBossRush5/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"

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

	m_Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	m_HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	m_HealthBarWidget->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (m_HealthBarWidget)
		m_HealthBarWidget->SetVisibility(false);
}

void AEnemy::Die()
{
	//TODO: Play Death Montage
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && m_DeathMontage)
	{
		AnimInstance->Montage_Play(m_DeathMontage);
		const int32 Selection = FMath::RandRange(0,5);
		FString sName = "Death" + FString::FromInt(Selection+1);
		FName SectionName = FName(*sName);
		switch (Selection)
		{
		case 0:
			m_DeathPose = EDeathPose::EDP_Death1;
			break;
		case 1:
			m_DeathPose = EDeathPose::EDP_Death2;
			break;
		case 2:
			m_DeathPose = EDeathPose::EDP_Death3;
			break;
		case 3:
			m_DeathPose = EDeathPose::EDP_Death4;
			break;
		case 4:
			m_DeathPose = EDeathPose::EDP_Death5;
			break;
		case 5:
			m_DeathPose = EDeathPose::EDP_Death6;
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, m_DeathMontage);
	}

	if (m_HealthBarWidget)
		m_HealthBarWidget->SetVisibility(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetLifeSpan(3.0f);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (m_CombatTarget)
	{
		const double DistanceToTarget = (m_CombatTarget->GetActorLocation() - GetActorLocation()).Size();
		if (DistanceToTarget > m_CombatRadius)
		{
			m_CombatTarget = nullptr;
			if (m_HealthBarWidget)
				m_HealthBarWidget->SetVisibility(false);
		}
	}
}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	//DRAW_SPHERE_COLOR(ImpactPoint, FColor::Orange);
	if (m_HealthBarWidget)
	{
		m_HealthBarWidget->SetVisibility(true);
	}
	if (m_Attributes && m_Attributes->IsAlive())
	{
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		Die();
	}
	if (m_HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			m_HitSound,
			ImpactPoint
		);
	}
	if (m_HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			m_HitParticles,
			ImpactPoint
		);
	}
}

void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	// Lower Impact Point to the Enemy's Actor Location Z
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// Forward * ToHit = |Forward||ToHit| * cos(theta)
	// |Forward| = 1, |ToHit| = 1, so Forward * ToHit = cos(theta)
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	// Take the inverse cosine (arc-cosine) of cos(theta) to get theta
	double Theta = FMath::Acos(CosTheta);
	// convert from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);

	// if CrossProduct points down, Theta should be negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}
	
	FName Section("FromBack");

	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}

	PlayHitReactMontage(Section);
	
	// UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 100.f, 5.f, FColor::Blue, 5.f);
	//
	// if (GEngine)
	// {
	// 	GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Theta: %f"), Theta));
	// }
	// UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);
	// UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.f);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
						 AController* EventInstigator, AActor* DamageCauser)
{
	if (m_Attributes)
	{
		m_Attributes->ReceiveDamage(DamageAmount);
			
		if (m_HealthBarWidget)
		{
			m_HealthBarWidget->SetHealthPercent(m_Attributes->GetHealthPercent());
		}
	}
	m_CombatTarget = EventInstigator->GetPawn();
	return DamageAmount;
}

void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && m_HitReactMontage)
	{
		AnimInstance->Montage_Play(m_HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, m_HitReactMontage);
	}
}

void AEnemy::PlayDeathMontage(const FName& SectionName)
{

}



