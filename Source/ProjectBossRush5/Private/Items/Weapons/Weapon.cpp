// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Weapons/Weapon.h"

#include "NiagaraComponent.h"
#include "Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::PlayEquipSound()
{
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquipSound,
			GetActorLocation()
		);
	}
}

void AWeapon::DisableSphereCollision()
{
	if (m_Sphere)
	{
		m_Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::DeactivateEmbers()
{
	if (m_EmbersEffect)
	{
		m_EmbersEffect->Deactivate();
	}
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	m_ItemState = EItemState::EIS_Equipped;
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);
	AttachMeshToSocket(InParent, InSocketName);
	DisableSphereCollision();
	PlayEquipSound();
	DeactivateEmbers();
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	m_ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::ExecuteGetHit(FHitResult BoxHit)
{
	//On Hit Trigger Here
	IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
	if (HitInterface)
	{
		HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint);
	}
}

bool AWeapon::ActorIsSameType(AActor* OtherActor)
{
	if (GetOwner()->ActorHasTag(TEXT("Enemy")) && OtherActor->ActorHasTag(TEXT("Enemy"))) return true;
	return false;
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ActorIsSameType(OtherActor)
	|| OtherActor == GetOwner())
		return;
	
	
	FHitResult BoxHit;
	BoxTrace(BoxHit);

	if (BoxHit.GetActor() && BoxHit.GetActor() == OtherActor)
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *BoxHit.GetActor()->GetName());
		
		if (ActorIsSameType(BoxHit.GetActor())
		|| BoxHit.GetActor() == GetOwner()) return;
		
		// if (GetOwner() == nullptr || GetInstigator() == nullptr)
		// 	return;

		UGameplayStatics::ApplyDamage(
			OtherActor,
			Damage,
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass()
		);
		
		ExecuteGetHit(BoxHit);
		
		CreateFields(BoxHit.ImpactPoint);
	}
}

void AWeapon::BoxTrace(FHitResult& BoxHit)
{
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	
	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}
	
	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		m_BoxTraceExtent,
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		m_bShowBoxDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		BoxHit,
		true
	);
	IgnoreActors.AddUnique(BoxHit.GetActor());
}
