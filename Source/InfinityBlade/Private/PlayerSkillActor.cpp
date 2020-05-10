// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerSkillActor.h"
#include "IBSkillProjectileController.h"



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
	GetCapsuleComponent()->SetCapsuleSize(10.0f, 4.0f);
	//GetCapsuleComponent()->SetCapsuleHalfHeight(10.0f);
	//GetCapsuleComponent()->SetCapsuleRadius(10.0f);

	//GetCapsuleComponent()->SetWorldScale3D();
	RootComponent = GetCapsuleComponent();
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("Skill"));
	GetCharacterMovement()->MaxWalkSpeed = 4000.0f;
	GetCharacterMovement()->MaxAcceleration = 4000.0f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 4000.0f;
	GetCharacterMovement()->GroundFriction = 0.0f;
	GetCharacterMovement()->bRequestedMoveUseAcceleration = false;
	GetCharacterMovement()->bJustTeleported = false;
	GetCharacterMovement()->bCrouchMaintainsBaseLocation = true;
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//AIControllerClass = AIB_E_GREATERSPIDER_AIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	bStartMove = false;
	DeleteTime = 0.0f;
}

// Called when the game starts or when spawned
void APlayerSkillActor::BeginPlay()
{
	Super::BeginPlay();
	//EnableInput(IBSkillProjectileController);
	
}

// Called every frame
void APlayerSkillActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bStartMove)
	{
		DeleteTime += DeltaTime;
		if (DeleteTime > 1.5f)
			this->Destroy();
	}
}

// Called to bind functionality to input
void APlayerSkillActor::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void APlayerSkillActor::SetVelocity()
{
	//GetMovementComponent()->Velocity = FVector(400.0f, 400.0f, 0.0f);
}

void APlayerSkillActor::SetLoctation(FVector TargetPos)
{
	NavSystem = UNavigationSystem::GetNavigationSystem(GetWorld());
	NavSystem->SimpleMoveToLocation(GetController(), TargetPos);
	bStartMove = true;
}


