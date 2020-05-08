// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InfinityBlade.h"
#include "GameFramework/Character.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "PlayerSkillActor.generated.h"

UCLASS()
class INFINITYBLADE_API APlayerSkillActor : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APlayerSkillActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = Collision)
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, Category = Collision)
	UCapsuleComponent* SkillCapsule;

	UPROPERTY(VisibleAnywhere, Category = Visual)
	USkeletalMeshComponent* SkillMesh;

	UPROPERTY(VisibleAnywhere, Category = Movement)
	UMovementComponent* SkillMovement;

public:
	void SetVelocity();



};
