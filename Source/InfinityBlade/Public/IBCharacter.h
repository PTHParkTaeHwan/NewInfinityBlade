// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "InfinityBlade.h"
#include "GameFramework/Character.h"
#include "IBCharacter.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnAttackEndDelegate);


UCLASS()
class INFINITYBLADE_API AIBCharacter : public ACharacter
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	AIBCharacter();
	
//캐릭터 스테이트 관리
public:
	void SetCharacterState(ECharacterState NewState);
	ECharacterState GetCharacterState() const;
	int32 GetExp() const;

private:
	int32 AssetIndex = 0;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	ECharacterState CurrentState;
	
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = State, Meta = (AllowPrivateAccess = true))
	bool bIsPlayer;

	UPROPERTY()
	class AIBAIController* IBAIController;

	UPROPERTY()
	class AIBPlayerController* IBPlayerController;
//// end of state Management

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	enum class EControlMode
	{
		GTA,
		DEFENSE,
		NPC
	};

	void SetControlMode(EControlMode NewControlMode);
	EControlMode CurrentControlMode = EControlMode::GTA;
	FVector DirectionToMove = FVector::ZeroVector;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void PostInitializeComponents() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const&DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	//virtual void PossessedBy(AController* NewController) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	USkeletalMeshComponent* Weapon;
	
	UPROPERTY(VisibleAnywhere, Category = Stat)
	class UIBCharacterStatComponent* CharacterStat;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* HPBarWidget;	

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShake> MyShake;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShake> NewMyShake;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShake> CS_FirstSkill;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UCameraShake> CS_BasicHit;

	UPROPERTY(EditInstanceOnly, Category = Skill)
	TSubclassOf<class APlayerSkillActor> SkillActorClass;

	class APlayerSkillActor * NewSkillActor;

	//공격 모션
	void Attack();
	FOnAttackEndDelegate OnAttackEnd;

	//sound
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundCue* FSkillSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundCue* DodgeSound1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundCue* DodgeSound2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundCue* DodgeSound3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundCue* ShieldHitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundCue* PlayerHitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Audio, meta = (AllowPrivateAccess = "true"))
	class USoundCue* EnemyHitSound;

	   
private:
	struct S_AudioComponent
	{
		UAudioComponent* SkillSoundComponent;
	};
	std::vector<S_AudioComponent> vAuidoVector;
	std::vector<S_AudioComponent>::iterator viAuidoVector;
	int32 SE_SoundNum;

	UAudioComponent* DodgeSoundComponent1;
	UAudioComponent* DodgeSoundComponent2;
	UAudioComponent* DodgeSoundComponent3;

	UAudioComponent* ShieldHitSoundComponent;
	UAudioComponent* PlayerHitSoundComponent;
	UAudioComponent* EnemyHitSoundComponent;

public:
	bool GetIsRun();
	bool GetIsDefense();
	
	//무기 아이템 습득
	bool CanSetWeapon();
	void SetWeapon(class AIBWeapon* NewWeapon);
		
	
	UPROPERTY(VisibleAnywhere, Category = Weapon)
	class AIBWeapon* CurrentWeapon;

private:
	//캐릭터 움직임
	void UpDown(float NewAxisValue);
	void LeftRight(float NewAxisValue);
	//카메라 움직임
	void LookUp(float NewAxisValue);
	void Turn(float NewAxisValue);
	
	//캐릭터 걷기 모션
	void ModeChange();
	void RunChange();

	float ArmLengthTo = 0.0f;
	FRotator ArmRotationTo = FRotator::ZeroRotator;
	float ArmLengthSpeed = 0.0f;

	FVector ArmLocationTo = FVector::ZeroVector;
	float ArmLocationSpeed = 0.0f;

	FVector CameraLocationTo = FVector::ZeroVector;
	float CameraLocationSpeed = 0.0f;

	bool IsRun;
	bool CurrentShiftButtonOn;
	bool IsDefense;
	
	//Hit Motion
	bool IsHit;
	bool bHitRotator;
	bool bHitMotionBlendTime;
	FRotator TargetRot;
	FVector TargetPos = FVector::ZeroVector;

	//Dodge Motion
	bool bCanDodge;
	bool bSkipTakeDamage;
	void DodgeMotion();
public:
	void SetCanDodge(bool NewDodgeState);
	void SetSkipTakeDamage(bool NewState);
	bool GetSkipTakeDamage();

private:
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	//공격모션
	void AttackStartComboState();
	void AttackEndAttackState();

	//공격 충돌처리
	void AttackCheck();

	//캐릭터 에셋 로드
	void OnAssetLoadCompleted();
	FSoftObjectPath CharacterAssetToLoad = FSoftObjectPath(nullptr);
	TSharedPtr<struct FStreamableHandle> AssetStreamingHandle;

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsAttacking;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool CanNextCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	bool IsComboInputOn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 CurrentCombo;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	int32 MaxCombo;
			
	UPROPERTY()
	class UIBAnimInstance* IBAnim;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRange;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = Attack, Meta = (AllowPrivateAccess = true))
	float AttackRadius;

//파티클 시스템
public:

	UPROPERTY(VisibleAnywhere, Category = Effect)
	UParticleSystemComponent* HitEffect;

	UPROPERTY(VisibleAnywhere, Category = Effect)
	UParticleSystemComponent* ShieldHitEffect;

	UPROPERTY(VisibleAnywhere, Category = Skill)
	UParticleSystemComponent* SkillEffect_1;

	UPROPERTY(VisibleAnywhere, Category = Skill)
	UParticleSystemComponent* SkillEffect_1_Final;

	UPROPERTY(VisibleAnywhere, Category = Skill)
	UParticleSystemComponent* ShieldSkill;

	//스킬4


	UPROPERTY(VisibleAnywhere, Category = Skill)
	UParticleSystemComponent* TestParticle;

	UPROPERTY(VisibleAnywhere, Category = Skill)
	UParticleSystemComponent* TestParticle2;





//dead system
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = State, Meta = (AllowPrivateAccess = true))
	float DeadTimer;

	FTimerHandle DeadTimerHandle = {};

//AttackStep



private:
	void AttackStepAddLambda();
	void MoveAttackType1();
	void InitAttackStep();

	int32 AttackStepNum;
	bool IsAttackType_1Step;
	bool IsAttackType_2Step;

	//AttackMode
private:
	AttackStyle CurrentAttackStyle = AttackStyle::BASICATTACK;
	LSAttackMode CurrentAttackMode = LSAttackMode::NONE;
	float CheckIntervalTime;
	bool FirstAttackClick;
	bool SetAttackMode;
	bool TimeCheckStart;
	bool bBasicAttackMontage;
	bool bWeaponTypeSetUp;

	bool SecondAttackClick;
	bool CanComboAttack;
	int32 CurrentBasicAttackSection;

	bool bClawStepMoveOn;

public:
	LSAttackMode GetCurrentAttackMode();
	int32 GetCurrntCombo();
	AttackStyle GetCurrentAttackStyle();

	//skill
public:
	void InitSkillParticle();
	void InitFirstSkill();
	void InitSecondSkill();
	void InitForthSkill();
	void Skill_3();

	void FirstSkillAttackCheck(FVector ExplosionVector);
	void FirstSkillStepMove();
	void ForthSkillAttackCheck(FVector ExplosionVector);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector MuzzleOffset;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AIBSkillProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class APlayerSkillActor> PlayerSkillActor;


private:
	void SkillHub(float DeltaTime);

	//1st
	void InitGroundBurstSkillParameter();
	bool bFirstSkillEffect;
	bool bInitLoction;
	float EffectIntervalTime;
	int32 EffectNum;
	AIBSkillProjectile* Projectile;
	APlayerSkillActor* SkillActor;

	//2nd
	void InitShieldSkillParameter();
	bool bSecondSkillEffect;
	float ShieldSkillActiveTime;
	float fMaxShieldSkillTime;


	//4th
	void InitUltimateSkillParameter();
	bool bForthSkillEffect;
	bool bForthSkillMontagePlay;
	bool bBasicUltimateSkill;
	FVector SkillStartForwardLeftVector = FVector::ZeroVector;
	FVector SkillStartForwardRightVector = FVector::ZeroVector;
	FVector SkillStartBackVector = FVector::ZeroVector;
	FVector SkillStartBackLeftVector = FVector::ZeroVector;
	FVector SkillStartBackRightVector = FVector::ZeroVector;
	FVector SkillStartRightVector = FVector::ZeroVector;
	FVector SkillStartLeftVector = FVector::ZeroVector;

	struct UltimateParticle
	{
		UPROPERTY(VisibleAnywhere, Category = Skill)
		UParticleSystemComponent* FirstParticle;
		UPROPERTY(VisibleAnywhere, Category = Skill)
		UParticleSystemComponent* SecondParticle;
		FVector SkillStartVector;		
	};

	std::vector<UltimateParticle> m_vUSParticleVector;
	std::vector<UltimateParticle>::iterator m_viUSParticleVector;
	int ParticelNum;


	
	FVector SkillStartLocation = FVector::ZeroVector;
	FVector SkillStartForwardVector = FVector::ZeroVector;

public:
	//레벨업 관련
	UPROPERTY(VisibleAnywhere, Category = LevelUp)
	UParticleSystemComponent* LevelUpParticle;

	void PlayLevelUpParticle();


private:
	void InitCameraShakeParam();

	//테스트용 Parameter
private:
	void TestParameter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Testparameter, Meta = (AllowPrivateAccess = true))
	float TestFloat1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Testparameter, Meta = (AllowPrivateAccess = true))
	float TestFloat2; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Testparameter, Meta = (AllowPrivateAccess = true))
	float TestFloat3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Testparameter, Meta = (AllowPrivateAccess = true))
	float TestFloat4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Testparameter, Meta = (AllowPrivateAccess = true))
	float TestFloat5;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Testparameter, Meta = (AllowPrivateAccess = true))
	int32 TestInt1;
};
