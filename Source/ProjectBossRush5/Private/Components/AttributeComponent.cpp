// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AttributeComponent.h"

// Sets default values for this component's properties
UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	m_Health = FMath::Clamp(m_Health - Damage, 0, m_MaxHealth);
	if (m_Health <= 0)
	{
		//Die LOL;
	}
}

float UAttributeComponent::GetHealthPercent()
{
	return m_Health/m_MaxHealth;
}

bool UAttributeComponent::IsAlive()
{
	return m_Health > 0;
}


// Called every frame
void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

