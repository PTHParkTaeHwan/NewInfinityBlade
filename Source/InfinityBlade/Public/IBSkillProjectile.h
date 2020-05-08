// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InfinityBlade.h"
#include "GameFramework/Actor.h"
#include "Sphere.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "IBSkillProjectile.generated.h"

UCLASS()
class INFINITYBLADE_API AIBSkillProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIBSkillProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	USphereComponent* CollisionComponent;
	
	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere, Category = Visual)
	USkeletalMeshComponent* SkillMesh;


	void FireInDirection(FVector ShootDirection, float NewSpeed);
	void Setradius(float NewRadius);
	void SetProjectileGravityScale(float NewGravityScale);

private:
	FVector StartVector;
	bool bFire;
	float Speed;
};
