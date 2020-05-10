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

public:
	void SetVelocity();
	void SetLoctation(FVector TargetPos);

private:
	UPROPERTY()
	class AIBSkillProjectileController* IBSkillProjectileController;

	FVector GoalPos;
	bool bStartMove;
	float DeleteTime;
	UNavigationSystem* NavSystem;
};
