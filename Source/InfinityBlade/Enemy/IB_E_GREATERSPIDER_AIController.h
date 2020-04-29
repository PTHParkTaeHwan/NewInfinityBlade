// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InfinityBlade.h"
#include "AIController.h"
#include "IB_E_GREATERSPIDER_AIController.generated.h"


/**
 * 
 */
UCLASS()
class INFINITYBLADE_API AIB_E_GREATERSPIDER_AIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AIB_E_GREATERSPIDER_AIController();
	virtual void Possess(APawn* InPawn) override;

private:

	UPROPERTY()
	class UBehaviorTree* BTAsset;

	UPROPERTY()
	class UBlackboardData* BBAssset;

public:
	static const FName HomePosKey;
	static const FName PatrolPosKey;
	static const FName TargetKey;
	static const FName IsRoarKey;
	
	//���� �� �ൿ ���� ������
	static const FName IsStayHereKey;
	static const FName AttackOrIdleKey;
	static const FName LeftOrRightKey;
	static const FName LeftPosKey;
	static const FName RightPosKey;
	static const FName IsHitMotionKey;

	
	
};
