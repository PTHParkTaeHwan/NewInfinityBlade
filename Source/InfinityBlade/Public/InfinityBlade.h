// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

//#include "CoreMinimal.h"
#include "EngineMinimal.h"
#include <vector>

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	PREINIT,
	LOADING,
	READY,
	DEAD
};

enum class EnemyMode
{
	/*
	1. ��Ʈ�� ��忴�ٰ�
	2. �÷��̾ �߰��ϸ� �Լ��� �ѹ� ������
	3. �÷��̾�� �޷�����
	4. �÷��̾ ������ �� �ִ� �Ÿ��� �Ǹ� �������� �Ѿ��
	5. �÷��̾ �߽����� ����, ���������� �Դٰ���� �ϴٰ� �����Ѵ�.
	6. ������ ���۵Ǹ� �޺��� �����Ѵ�.
	7. �÷��̾��� ���ݿ� �ǰݴ��ϸ� �ǰݸ���� ������ �� �ٽ� �������� �Ѿ��.
	*/
	PATROL,
	TARGETON,
	TENTIONON,
	ATTACK,
	HIT
};

//�÷��̾� ���� Ÿ��
enum class WeaponType
{
	LONGSWORD,
	MIDDLESWORD,
	BASICSWORD
};

enum class LSAttackMode
{
	NONE,
	BASIC,
	COMBO,
	SKILL
};

enum class AttackStyle
{
	BASICATTACK,
	CLAW,
	ULITIMATE,	
};

DECLARE_LOG_CATEGORY_EXTERN(InfinityBlade, Log, All);
#define ABLOG_CALLINFO (FString(__FUNCTION__) + TEXT("(") + FString::FromInt(__LINE__)+TEXT(")"))
#define ABLOG_S(Verbosty) UE_LOG(InfinityBlade, Verbosity, TEXT("%s"), *ABLOG_CALLINFO)
#define ABLOG(Verbosity, Format, ...) UE_LOG(InfinityBlade, Verbosity, TEXT("%s%s"), *ABLOG_CALLINFO, *FString::Printf(Format, ##__VA_ARGS__))

#define ABCHECK(Expr, ...) { if(!(Expr)) { ABLOG(Error, TEXT("ASSERTION : %s"), TEXT("'"#Expr"'")); return __VA_ARGS__; }}