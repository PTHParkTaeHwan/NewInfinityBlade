// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerSkillActor.h"


// Sets default values
APlayerSkillActor::APlayerSkillActor()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Chicken(TEXT("/Game/InfinityBladeAdversaries/Enemy/Enemy_Chicken/S_Survival_CA_Chicken.S_Survival_CA_Chicken"));
	if (Chicken.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(Chicken.Object);
	}


	RootComponent = GetCapsuleComponent();
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("IBCharacter"));
	GetCharacterMovement()->MaxWalkSpeed = 400;
	GetCharacterMovement()->bJustTeleported = false;
	GetCharacterMovement()->bCrouchMaintainsBaseLocation = true;
}

// Called when the game starts or when spawned
void APlayerSkillActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayerSkillActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerSkillActor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerSkillActor::SetVelocity()
{
	GetMovementComponent()->Velocity = FVector(400.0f, 400.0f, 0.0f);
}

