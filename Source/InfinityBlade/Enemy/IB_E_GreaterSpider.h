// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InfinityBlade.h"
#include "GameFramework/Character.h"
#include "IB_E_GreaterSpider.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class INFINITYBLADE_API AIB_E_GreaterSpider : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AIB_E_GreaterSpider();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;

protected:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const&DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
public:
	void KnockBackMotion(AActor * DamageCauser);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//���� ���
	void Attack();
	FOnAttackEndDelegate OnAttackEnd;

	//�޺� ���� ����
	void SetCharacterInAttackRange(bool InAttackRange);
	void SetEnemyMode(EnemyMode NewMode);
	int32 GetCurrentCombo();
	bool GetIsAttackMontage();

	//HP Bar widget ����
public:
	UPROPERTY(VisibleAnywhere, Category = Stat)
	class UIB_E_GS_StatComponent* CharacterStat;

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* HPBarWidget;

	void SetHPBarWidgetHiddenInGame(bool NewStat);
	

private:
	UFUNCTION()
		void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//���ݸ��
	void AttackStartComboState();

	//���� �浹ó��
	void AttackCheck();

	//Dodge 
	void PlayerCheck();
	void IsDodgeCheck();



private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
		bool IsAttacking;

	/*UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;*/

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo;

	UPROPERTY()
	class UIB_E_GreaterSpider_AnimInstance* IB_E_GSAnim;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRadius;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CharacterInAttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool AnimNotify_NextAttackCheckOn;



	//roar ����
private:
	bool IsRoar;
	bool IsFirstRoar;
public:
	bool GetIsRoar();

	//tentnion mode ����
private:
	bool AttackOn;
	bool IsLeft;
	bool IsRight;
	bool IdleOn;
	bool IsLeftOrRightMove;
	float IdleTime;

	bool IsStayHere;
	bool AttackOrIdle;
	bool LeftOrRight;

public:
	void TentionModeInit();
	void SetTentionMode();
	bool GetIsStayHere();
	bool GetAttackOrIdle();
	bool GetLeftOrRight();
	bool GetIsAttacking();
	void SetIsAttacking(bool NewState);
	bool GetAttackOn();
	bool GetIsLeftOrRightMove();
	void SetbOrientRotationToMovement(bool NewRotation);
	void PlayLeftOrRightMontage();

	//dead State
private:
	bool DeadModeOn;

public:
	int32 GetExp() const;

	//��ƼŬ �ý���

public:
	UPROPERTY(VisibleAnywhere, Category = Effect)
	UParticleSystemComponent* HitEffect;

	//hit montion
public:
	bool GetHitMotionOn();

private:
	bool HitMotionOn;
	
	//�˹� motion ���� ����
	bool bKnockBackByBasicAttack;
	bool bKnockBackBySkill;
	float KnockBackTime;
	float MaxKnockBackTime;

	bool bPushedbySkill;

	void KnockBackMotionHub(float DeltaTime);

private:
	bool bInitWidgetLocation;
	
	//����Ʈ task ����
	bool bDetectSuccess;
public:
	void SetDetectState(bool NewState);
	bool GetDetectState();

	


//test parameter
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Testparameter, Meta = (AllowPrivateAccess = true))
	float TestFloat1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Testparameter, Meta = (AllowPrivateAccess = true))
	float TestFloat2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Testparameter, Meta = (AllowPrivateAccess = true))
	float TestFloat3;

};
