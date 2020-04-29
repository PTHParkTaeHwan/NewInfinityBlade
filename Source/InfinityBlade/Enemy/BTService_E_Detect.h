// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InfinityBlade.h"
#include "BehaviorTree/BTService.h"
#include "BTService_E_Detect.generated.h"

/**
 * 
 */
UCLASS()
class INFINITYBLADE_API UBTService_E_Detect : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_E_Detect();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	float DetectRadius;

	
	
};
