// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Bird.generated.h"

//Forward Declare
class UCapsuleComponent;
class USkeletalMeshComponent;
class USpringArmComponent;
class UCameraComponent;


UCLASS()
class PROJECTBOSSRUSH5_API ABird : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ABird();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	
private:
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* m_CapsuleCollider;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* m_BirdMesh;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* m_CameraBoom;
	
	UPROPERTY(VisibleAnywhere)
	UCameraComponent* m_ViewCamera;
};
