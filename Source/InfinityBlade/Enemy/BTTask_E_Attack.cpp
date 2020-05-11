// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_E_Attack.h"
#include "IB_E_GREATERSPIDER_AIController.h"
#include "IB_E_GreaterSpider.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "IBCharacter.h"

UBTTask_E_Attack::UBTTask_E_Attack()
{
	NodeName = TEXT("E_Attack_GreaterSpider");
	bNotifyTick = true;
	IsAttacking = false;
}

EBTNodeResult::Type UBTTask_E_Attack::ExecuteTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);
	auto IBGreaterSpider = Cast<AIB_E_GreaterSpider>(OwnerComp.GetAIOwner()->GetPawn());
	if (nullptr == IBGreaterSpider)
		return EBTNodeResult::Failed;

	if (!IBGreaterSpider->GetAttackOn())
	{
		IBGreaterSpider->Attack();
		IsAttacking = true;
		return EBTNodeResult::InProgress;
	}
	else if (IBGreaterSpider->GetAttackOn())
	{
		return EBTNodeResult::InProgress;
	}
	
	//IBGreaterSpider->TentionModeInit();
	/*
	IBGreaterSpider->Attack();
	if (!IBGreaterSpider->GetAttackOn())
	{
		IBGreaterSpider->TentionModeInit();
		return EBTNodeResult::Failed;
	}
	IsAttacking = true;
	*/
	   	
	IBGreaterSpider->OnAttackEnd.AddLambda([this]() -> void {
		IsAttacking = false;
	});
	return EBTNodeResult::InProgress;

}

void UBTTask_E_Attack::TickTask(UBehaviorTreeComponent & OwnerComp, uint8 * NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);
	if (IsAttacking)
	{
		AIB_E_GreaterSpider* IBGreaterSpider = Cast<AIB_E_GreaterSpider>(OwnerComp.GetAIOwner()->GetPawn());
		AIBCharacter* Target = Cast<AIBCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
		float TempDistance = Target->GetDistanceTo(IBGreaterSpider);
		if (TempDistance >= 350.0f)
		{
			IBGreaterSpider->TentionModeInit();
			IBGreaterSpider->SetIsAttacking(false);
			IsAttacking = false;
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	if (!IsAttacking)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

}

