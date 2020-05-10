// Fill out your copyright notice in the Description page of Project Settings.

#include "IBSkillProjectile.h"
#include "IBSkillProjectileController.h"


// Sets default values
AIBSkillProjectile::AIBSkillProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SkillMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkillMeshComponent"));
	RootComponent = CollisionComponent;
	CollisionComponent->InitSphereRadius(1.0f);

	CollisionComponent->bDynamicObstacle = false;
	//CollisionComponent->SetRelativeScale3D(FVector(0.1f, 0.1f, 0.1f));


	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->SetUpdatedComponent(CollisionComponent);
	ProjectileMovementComponent->InitialSpeed = 300.0f;
	ProjectileMovementComponent->MaxSpeed = 300.0f;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = true;
	ProjectileMovementComponent->Bounciness = 0.3f;
	ProjectileMovementComponent->ProjectileGravityScale = 1.0f;
	SkillMesh->SetupAttachment(CollisionComponent);
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("/Game/InfinityBladeAdversaries/Enemy/Enemy_Chicken/S_Survival_CA_Chicken.S_Survival_CA_Chicken"));
	if (SM.Succeeded())
	{
		SkillMesh->SetSkeletalMesh(SM.Object);
	}

	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("IBCharacter"));
	ProjectileMovementComponent->bIsSliding = true;

	bFire = false;
	

	//AIControllerClass = AIB_E_GREATERSPIDER_AIController::StaticClass();
	//AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	EnableInput(IBSkillProjectileController);

}

// Called when the game starts or when spawned
void AIBSkillProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AIBSkillProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bFire)
	{
		ProjectileMovementComponent->Velocity = StartVector * Speed;
	}

}

void AIBSkillProjectile::FireInDirection(FVector ShootDirection, float NewSpeed)
{
	ProjectileMovementComponent->Velocity = ShootDirection * ProjectileMovementComponent->InitialSpeed;
	StartVector = ShootDirection;
	bFire = true;
	Speed = NewSpeed;

	UNavigationSystem* NavSystem = UNavigationSystem::GetNavigationSystem(GetWorld());
	NavSystem->SimpleMoveToLocation(IBSkillProjectileController, GetActorLocation() + GetActorForwardVector()*400.0f);

	

}
void AIBSkillProjectile::Setradius(float NewRadius)
{
	CollisionComponent->InitSphereRadius(NewRadius);

}

void AIBSkillProjectile::SetProjectileGravityScale(float NewGravityScale)
{
	ProjectileMovementComponent->ProjectileGravityScale = NewGravityScale;
}


