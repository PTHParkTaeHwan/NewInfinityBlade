// Fill out your copyright notice in the Description page of Project Settings.

#include "IBCharacter.h"
#include "IBAnimInstance.h"
#include "IBWeapon.h"
#include "IBCharacterStatComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/WidgetComponent.h"
#include "IBCharacterWidget.h"
#include "IBAIController.h"
#include "IBCharacterSetting.h"
#include "IBGameInstance.h"
#include "IBPlayerController.h"
#include "IBPlayerState.h"
#include "IBHUDWidget.h"
#include "Enemy/IB_E_GreaterSpider.h"


// Sets default values
AIBCharacter::AIBCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//Pawn�� ĸ��, �޽�, �����Ʈ�� ������ �� �ִ� �Լ��� �ֱ� ������ ������ �ϰ� ī�޶� ���� ������ش�.
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SPRINGARM"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("CAMERA"));
	CharacterStat = CreateDefaultSubobject<UIBCharacterStatComponent>(TEXT("CHARACTERSTAT"));
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));

	SpringArm->SetupAttachment(GetCapsuleComponent());
	Camera->SetupAttachment(SpringArm);
	HPBarWidget->SetupAttachment(GetMesh());

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	SpringArm->TargetArmLength = 690.0f;
	SpringArm->SetRelativeRotation(FRotator(-15.0f, 0.0f, 0.0f));
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 85.0f));
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CardBoard(TEXT("/Game/InfinityBladeWarriors/Character/CompleteCharacters/SK_CharM_Bladed.SK_CharM_Bladed"));
	if (CardBoard.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(CardBoard.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	//�ִϸ��̼� �ν��Ͻ� ��������
	static ConstructorHelpers::FClassFinder<UAnimInstance> WARRIOR_ANIM(TEXT("/Game/Book/Animations/WarriorAnimBlueprint.WarriorAnimBlueprint_C"));
	if (WARRIOR_ANIM.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(WARRIOR_ANIM.Class);
	}

	//ī�޶� ��� ��ȯ
	SetControlMode(EControlMode::GTA);
	ArmLengthSpeed = 3.0f;
	ArmLocationSpeed = 5.0f;
	CameraLocationSpeed = 1.5f;

	//�ȱ� ��� ��ȯ
	IsRun = false;
	CurrentShiftButtonOn = false;
	GetCharacterMovement()->MaxWalkSpeed = 400;
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	
	//���� ��� ����
	IsAttacking = false;
	bBasicAttackMontage = true;
	bWeaponTypeSetUp = false;
		
	//���� �޺� ����
	MaxCombo = 4;
	CurrentBasicAttackSection = 1;
	AttackEndAttackState();

	//�ݸ��� ������ ����
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("IBCharacter"));

	//���� ���� �����
	AttackRange = 150.0f;
	AttackRadius = 90.0f;

	//��ƼŬ �ý���
	FirstHitEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HITEFFECT"));
	FirstHitEffect->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_HIT(TEXT("/Game/InfinityBladeEffects/Effects/FX_Combat_Base/Impact/P_ImpactSpark.P_ImpactSpark"));
	if (P_HIT.Succeeded())
	{
		FirstHitEffect->SetTemplate(P_HIT.Object);
		FirstHitEffect->bAutoActivate = false;
	}

	HitEffect = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HITEFFECTTEST"));
	HitEffect->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_HITTEST(TEXT("/Game/InfinityBladeEffects/Effects/FX_Skill_Leap/P_Skill_Leap_Fire_Impact_Suction.P_Skill_Leap_Fire_Impact_Suction"));
	if (P_HITTEST.Succeeded())
	{
		HitEffect->SetTemplate(P_HITTEST.Object);
		HitEffect->bAutoActivate = false;
	}

	//��ų ����
	InitSkillParticle();
	InitGroundBurstSkillParameter();;
	InitShieldSkillParameter();
	InitUltimateSkillParameter();

	//HP, SE UI
	HPBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 180.0f));
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	static ConstructorHelpers::FClassFinder<UUserWidget> UI_HUD(TEXT("/Game/Book/UI/UI_HPUIBar.UI_HPUIBar_C"));
	if (UI_HUD.Succeeded())
	{
		HPBarWidget->SetWidgetClass(UI_HUD.Class);
		HPBarWidget->SetDrawSize(FVector2D(150.0f, 50.0f));
	}

	//AIControllerClass = AIBAIController::StaticClass();
	//AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	auto DefaultSetting = GetDefault<UIBCharacterSetting>();
	if (DefaultSetting->CharacterAssets.Num() > 0)
	{
		for (auto CharacterAsset : DefaultSetting->CharacterAssets)
		{
			//ABLOG(Warning, TEXT("Character Asset : %s"), *CharacterAsset.ToString());
		}
	}

	//State Management
	SetActorHiddenInGame(true);
	HPBarWidget->SetHiddenInGame(true);
	bCanBeDamaged = false;

	//dead System
	DeadTimer = 3.0f;

	//���� ����Ʈ test
	
	//AttackStep
	InitAttackStep();
	TestParameter();
}
void AIBCharacter::SetCharacterState(ECharacterState NewState)
{
	ABCHECK(CurrentState != NewState);
	CurrentState = NewState;

	switch (CurrentState)
	{
	case ECharacterState::LOADING:
		if (bIsPlayer)
		{
			DisableInput(IBPlayerController);

			IBPlayerController->GetHUDWidget()->BindCharacterStat(CharacterStat);

			auto IBPlayerState = Cast<AIBPlayerState>(PlayerState);
			ABCHECK(nullptr != IBPlayerState);
			CharacterStat->SetNewLevel(IBPlayerState->GetCharacterLevel());
		}

		SetActorHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(true);
		bCanBeDamaged = false;
		break;
	case ECharacterState::READY:
	{
		SetActorHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(false);
		bCanBeDamaged = true;
		CharacterStat->OnHPIsZero.AddLambda([this]() -> void {
			SetCharacterState(ECharacterState::DEAD);
		});

		auto CharacterWidget = Cast<UIBCharacterWidget>(HPBarWidget->GetUserWidgetObject());
		ABCHECK(nullptr != CharacterWidget);
		CharacterWidget->BindCharacterStat(CharacterStat);

		if (bIsPlayer)
		{
			SetControlMode(EControlMode::GTA);
			EnableInput(IBPlayerController);
		}
		else
		{
			SetControlMode(EControlMode::NPC);
			IBAIController->RunAI();
		}

		break; 
	}
	case ECharacterState::DEAD:
		SetActorEnableCollision(false);
		GetMesh()->SetHiddenInGame(false);
		HPBarWidget->SetHiddenInGame(true);
		IBAnim->SetDeadAnim();
		bCanBeDamaged = false;

		if (bIsPlayer)
		{
			DisableInput(IBPlayerController);
		}
		else
		{
			IBAIController->StopAI();
		}

		GetWorld()->GetTimerManager().SetTimer(DeadTimerHandle, FTimerDelegate::CreateLambda([this]() ->void {
			if (bIsPlayer)
			{
				//AIBPlayerController->RestartLevel();
			}
			else
			{
				Destroy();
			}
		}), DeadTimer, false);
		break;
	}
}
ECharacterState AIBCharacter::GetCharacterState() const
{
	return CurrentState;
}
int32 AIBCharacter::GetExp() const
{
	return CharacterStat->GetDropExp();
}
// Called when the game starts or when spawned
void AIBCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	bIsPlayer = IsPlayerControlled();

	if (bIsPlayer)
	{
		IBPlayerController = Cast<AIBPlayerController>(GetController());
		ABCHECK(nullptr != IBPlayerController);
	}
	else
	{
		IBAIController = Cast<AIBAIController>(GetController());
		ABCHECK(nullptr != IBAIController);
	}
	
	auto DefaultSetting = GetDefault<UIBCharacterSetting>();
	
	if (bIsPlayer)
	{
		auto IBPlayerState = Cast<AIBPlayerState>(PlayerState);
		ABCHECK(nullptr != IBPlayerState);
		AssetIndex = IBPlayerState->GetCharacterIndex();
	}
	else
	{
		AssetIndex = FMath::RandRange(0, DefaultSetting->CharacterAssets.Num() - 1);
	}
	
	CharacterAssetToLoad = DefaultSetting->CharacterAssets[AssetIndex];
	auto IBGameInstance = Cast<UIBGameInstance>(GetGameInstance());
	ABCHECK(nullptr != IBGameInstance);
	AssetStreamingHandle = IBGameInstance->StreamableManager.RequestAsyncLoad(CharacterAssetToLoad, FStreamableDelegate::CreateUObject(this, &AIBCharacter::OnAssetLoadCompleted));
	SetCharacterState(ECharacterState::LOADING);
}
void AIBCharacter::SetControlMode(EControlMode NewControlMode)
{
	CurrentControlMode = NewControlMode;
	
	switch (CurrentControlMode)
	{
	case EControlMode::GTA:
		IsDefense = false;
		ArmLengthTo = 700.0f;
		CameraLocationTo = FVector(0.0f, 0.0f, 0.0f);
		SpringArm->SetRelativeRotation(FRotator::ZeroRotator);
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = true;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		break;
	case EControlMode::DEFENSE:
		IsDefense = true;
		ArmLengthTo = 400.0f;
		CameraLocationTo = FVector(0.0f, -30.0f, 50.0f);
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bInheritPitch = true;
		SpringArm->bInheritRoll = true;
		SpringArm->bInheritYaw = true;
		SpringArm->bDoCollisionTest = true;
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 720.0f, 0.0f);
		break;
	case EControlMode::NPC:
		bUseControllerRotationYaw = false;
		GetCharacterMovement()->bUseControllerDesiredRotation = false;
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0.0f, 480.0f, 0.0f);
		break;
	}
}
// Called every frame
void AIBCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);	
	//SpringArm->TargetArmLength = FMath::FInterpTo(SpringArm->TargetArmLength, ArmLengthTo, DeltaTime, ArmLengthSpeed);
	//Camera->RelativeLocation = FMath::VInterpTo(Camera->RelativeLocation, CameraLocationTo, DeltaTime, CameraLocationSpeed);

	RunChange();

	switch (CurrentControlMode)
	{
	case AIBCharacter::EControlMode::DEFENSE:
		break;
	}
	
	//AttackStep
	MoveAttackType1();
	
	//�Ϲ�, �޺� ���� ����
	if (TimeCheckStart) CheckIntervalTime += DeltaTime;
	if (CheckIntervalTime > 0.17f && !SetAttackMode)
	{
		TimeCheckStart = false;
		SetAttackMode = true;
		CurrentAttackMode = LSAttackMode::BASIC;
		Attack();
	}	

	SkillHub(DeltaTime);
	SpringArm->SetRelativeRotation(FRotator(TestFloat1, 0.0f, 0.0f));

}
void AIBCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	IBAnim = Cast<UIBAnimInstance>(GetMesh()->GetAnimInstance());
	ABCHECK(nullptr != IBAnim);
	//��ũ�θ� Ȱ���� �ִ� �ν��Ͻ��� OnMontageEnded ��������Ʈ�� �츮�� ������ OnAttackMontageEnded�� �����Ѵ�.
	IBAnim->OnMontageEnded.AddDynamic(this, &AIBCharacter::OnAttackMontageEnded);

	IBAnim->OnNextAttackCheck.AddLambda([this]() -> void {
		if (CanNextCombo)
		{
			IsComboInputOn = true;
			CanNextCombo = false;	
			CurrentCombo++;
			IBAnim->PlayAttackMontage();
			IBAnim->JumpToAttackMontageSection(CurrentCombo);
			
		}
	});

	IBAnim->OnAttackHitCheck.AddUObject(this, &AIBCharacter::AttackCheck);

	CharacterStat->OnHPIsZero.AddLambda([this]() -> void {
		IBAnim->SetDeadAnim();
		SetActorEnableCollision(false);
	});

	auto CharacterWidget = Cast<UIBCharacterWidget>(HPBarWidget->GetUserWidgetObject());
	if (nullptr != CharacterWidget)
	{
		CharacterWidget->BindCharacterStat(CharacterStat);
	}
	AttackStepAddLambda();
	IBAnim->FOnFirstSkillStartCheck.AddLambda([this]() -> void {
		bFirstSkillEffect = true;
		SkillStartLocation = GetActorLocation() + FVector(0.0f, 0.0f, -90.0f);
		SkillStartForwardVector = GetActorForwardVector();
		if (ShieldSkill->IsActive()) ShieldSkill->SetVisibility(true);
	});

	IBAnim->FOnSecondSkillDoneCheck.AddLambda([this]() -> void {
		IsAttacking = false;
	});
	IBAnim->FOnForthSkillStartCheck.AddLambda([this]()->void {
		bForthSkillEffect = true;
	});
}
float AIBCharacter::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	float FinalDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	FirstHitEffect->Activate(true);
	
	//�׽�Ʈ��!!!!!
	FinalDamage = 0.0f;
	switch (CurrentControlMode)
	{
	case AIBCharacter::EControlMode::GTA:
		CharacterStat->SetDamage(FinalDamage, false);
		break;
	case AIBCharacter::EControlMode::DEFENSE:
		CharacterStat->SetDamage(FinalDamage, true);
		break;
	}
	if (CurrentControlMode == AIBCharacter::EControlMode::NPC)
	{
		CharacterStat->SetDamage(FinalDamage, false);
	}

	if (CurrentState == ECharacterState::DEAD)
	{
		if (EventInstigator->IsPlayerController())
		{
			auto IBPlayerController = Cast<AIBPlayerController>(EventInstigator);
			ABCHECK(nullptr != IBPlayerController, 0.0f);
			IBPlayerController->NPCKill(this);
		}
	}
	return FinalDamage;
}
// Called to bind functionality to input
void AIBCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	PlayerInputComponent->BindAxis(TEXT("UpDown"), this, &AIBCharacter::UpDown);
	PlayerInputComponent->BindAxis(TEXT("LeftRight"), this, &AIBCharacter::LeftRight);
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AIBCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AIBCharacter::Turn);
	PlayerInputComponent->BindAction(TEXT("DefenseMode"), EInputEvent::IE_Pressed, this, &AIBCharacter::ModeChange);
	PlayerInputComponent->BindAction(TEXT("DefenseMode"), EInputEvent::IE_Released, this, &AIBCharacter::ModeChange);
	PlayerInputComponent->BindAction(TEXT("Jump"), EInputEvent::IE_Pressed, this, &AIBCharacter::Jump);
	
	PlayerInputComponent->BindAction(TEXT("Runing"), EInputEvent::IE_Pressed, this, &AIBCharacter::ShiftButtonChange);
	PlayerInputComponent->BindAction(TEXT("Runing"), EInputEvent::IE_Released, this, &AIBCharacter::ShiftButtonChange);
	PlayerInputComponent->BindAction(TEXT("Attack"), EInputEvent::IE_Pressed, this, &AIBCharacter::Attack);
	PlayerInputComponent->BindAction(TEXT("Skill_1"), EInputEvent::IE_Pressed, this, &AIBCharacter::InitFirstSkill);
	PlayerInputComponent->BindAction(TEXT("Skill_2"), EInputEvent::IE_Pressed, this, &AIBCharacter::InitSecondSkill);
	PlayerInputComponent->BindAction(TEXT("Skill_3"), EInputEvent::IE_Pressed, this, &AIBCharacter::Skill_3);
	PlayerInputComponent->BindAction(TEXT("Skill_4"), EInputEvent::IE_Pressed, this, &AIBCharacter::InitForthSkill);
}
bool AIBCharacter::GetIsRun()
{
	return IsRun;
}
bool AIBCharacter::GetIsDefense()
{
	return IsDefense;
}
bool AIBCharacter::CanSetWeapon()
{
	return (nullptr == CurrentWeapon);
}
void AIBCharacter::SetWeapon(AIBWeapon * NewWeapon)
{
	ABCHECK(nullptr != NewWeapon && nullptr == CurrentWeapon);
	FName WeaponSocket(TEXT("hand_rSocket"));
	if (nullptr != NewWeapon)
	{
		NewWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocket);
		NewWeapon->SetOwner(this);
		CurrentWeapon = NewWeapon;
		SetHitEffect(CurrentWeapon);
	}

}
void AIBCharacter::UpDown(float NewAxisValue)
{
	if(!IsAttacking) AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
	else
	{
		//AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue);
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::X), NewAxisValue/100);
		//SetActorLocation(FMath::VInterpTo(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 30.0f, GetWorld()->GetDeltaSeconds(), 2.0f));
	}
}
void AIBCharacter::LeftRight(float NewAxisValue)
{
	if (!IsAttacking) AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);
	else
	{
		//AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue);
		AddMovementInput(FRotationMatrix(GetControlRotation()).GetUnitAxis(EAxis::Y), NewAxisValue / 100);
	}
}
void AIBCharacter::LookUp(float NewAxisValue)
{
	//Y��
	AddControllerPitchInput(NewAxisValue);
}
void AIBCharacter::Turn(float NewAxisValue)
{
	//Z��
	AddControllerYawInput(NewAxisValue);
}
void AIBCharacter::ModeChange()
{
	if (CurrentControlMode == EControlMode::GTA)
	{
		SetControlMode(EControlMode::DEFENSE);
	}
	else if (CurrentControlMode == EControlMode::DEFENSE)
	{
		SetControlMode(EControlMode::GTA);
	}
}
void AIBCharacter::RunChange()
{
	switch (CurrentControlMode)
	{
	case AIBCharacter::EControlMode::GTA:
		if (CurrentShiftButtonOn && this->GetVelocity().Size() > 0)
		{
			IsRun = true;
			GetCharacterMovement()->MaxWalkSpeed = 600;
		}
		else if (!CurrentShiftButtonOn && this->GetVelocity().Size() > 0)
		{
			IsRun = false;
			GetCharacterMovement()->MaxWalkSpeed = 400;
		}
		else
		{
			IsRun = false;
			GetCharacterMovement()->MaxWalkSpeed = 400;
		}
		break;
	case AIBCharacter::EControlMode::DEFENSE:
		GetCharacterMovement()->MaxWalkSpeed = 200;
		break;
	}
	
}
void AIBCharacter::ShiftButtonChange()
{
	if (!CurrentShiftButtonOn)
	{
		CurrentShiftButtonOn = true;
		IsAttacking = false;
		AttackEndAttackState();
		IBAnim->StopAttackMontage();
	}
	else if (CurrentShiftButtonOn)
	{
		CurrentShiftButtonOn = false;
	}
}
void AIBCharacter::Attack()
{	
	if (bBasicAttackMontage)
	{
		if (!IsAttacking)
		{
			IsAttacking = true;
			if (!bWeaponTypeSetUp)
			{
				bWeaponTypeSetUp = true;
				IBAnim->SetAttackMontageType(WeaponType::BASICSWORD);
			}
			AttackStartComboState();
			IBAnim->PlayAttackMontage();
			IBAnim->JumpToAttackMontageSection(CurrentCombo);

		}
		else
		{
			if (!CanNextCombo) CanNextCombo = true;
		}
	}
	else if (!bBasicAttackMontage)
	{
		if (!SetAttackMode)
		{
			if (!FirstAttackClick)
			{
				FirstAttackClick = true;
				TimeCheckStart = true;
			}
			else if (FirstAttackClick && CheckIntervalTime <= 0.17f)
			{
				TimeCheckStart = false;
				SetAttackMode = true;
				CurrentAttackMode = LSAttackMode::COMBO;
			}
		}
		switch (CurrentControlMode)
		{
		case AIBCharacter::EControlMode::GTA:
			switch (CurrentAttackMode)
			{
			case LSAttackMode::BASIC:
				if (!IsAttacking)
				{
					IsAttacking = true;
					IBAnim->PlayBasicAttackNontage();
					IBAnim->JumpToAttackMontageSection(CurrentBasicAttackSection);
					CurrentBasicAttackSection++;
					if (CurrentBasicAttackSection > 3) CurrentBasicAttackSection = 1;
				}
				break;
			case LSAttackMode::COMBO:
				if (IsAttacking)
				{
					ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 1, MaxCombo));
					if (CanNextCombo)
					{
						IsComboInputOn = true;
					}
				}
				else
				{
					ABCHECK(CurrentCombo == 0);
					AttackStartComboState();
					IBAnim->PlayAttackMontage();
					IBAnim->JumpToAttackMontageSection(CurrentCombo);
					IsAttacking = true;
				}
				break;
			case LSAttackMode::SKILL:
				break;
			}
			break;
		}
	}
}

void AIBCharacter::OnAttackMontageEnded(UAnimMontage * Montage, bool bInterrupted)
{
	if (IsComboInputOn)
	{
		IsComboInputOn = false;
	}
	else
	{
		IsAttacking = false;
		CurrentCombo = 0;
	}

	//AttackEndAttackState();
	if (!bBasicAttackMontage)
	{
		switch (CurrentAttackMode)
		{
		case LSAttackMode::BASIC:
			IsAttacking = false;
			break;
		case LSAttackMode::COMBO:
			if (IsComboInputOn && CurrentCombo < 4)
			{
				AttackStartComboState();
				IBAnim->PlayAttackMontage();
				IBAnim->JumpToAttackMontageSection(CurrentCombo);
			}
			else if (!IsComboInputOn)
			{
				ABCHECK(IsAttacking);
				ABCHECK(CurrentCombo > 0);
				IsAttacking = false;
				//OnAttackEnd.Broadcast();
			}
			break;
		case LSAttackMode::SKILL:
			break;
		}
	}
}
void AIBCharacter::AttackStartComboState()
{
	CanNextCombo = false;
	IsComboInputOn = false;
	ABCHECK(FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1));
	if (bBasicAttackMontage)
	{
		CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
	}
	if (!bBasicAttackMontage)
	{
		CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
	}
}
void AIBCharacter::AttackEndAttackState()
{
	//�Ϲ�, �޺� Ȯ�ο� ������
	CheckIntervalTime = 0.0f;
	FirstAttackClick = false;
	TimeCheckStart = false;
	SetAttackMode = false;	   
	   
	//�޺����ݿ� ������
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;

	CurrentAttackMode = LSAttackMode::NONE;
}
void AIBCharacter::AttackCheck()
{
	FCollisionQueryParams Params(NAME_None, false, this);
	TArray<FHitResult> HitResults;
	bool bResults = GetWorld()->SweepMultiByChannel(HitResults,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),
		Params);

#if ENABLE_DRAW_DEBUG

	FVector TraceVec = GetActorForwardVector() * AttackRange;
	FVector Center = GetActorLocation() + TraceVec * 0.5f;
	float HalfHeight = AttackRange * 0.5f + AttackRadius;
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TraceVec).ToQuat();
	FColor DrawColor = bResults ? FColor::Green : FColor::Red;
	float DebugLifeTime = 2.0f;

	DrawDebugCapsule(GetWorld(),
		Center,
		HalfHeight,
		AttackRadius,
		CapsuleRot,
		DrawColor,
		false,
		DebugLifeTime);

#endif
	if (bResults)
	{
		for (FHitResult HitResult : HitResults)
		{
			if (CurrentCombo < 4)
			{
				FDamageEvent DamageEvent;
				HitResult.Actor->TakeDamage(CharacterStat->GetAttack(), DamageEvent, GetController(), this);
			}
			else
			{
				FDamageEvent DamageEvent;
				HitResult.Actor->TakeDamage(CharacterStat->GetAttack()*2, DamageEvent, GetController(), this);
				AIB_E_GreaterSpider* HitActor = Cast<AIB_E_GreaterSpider>(HitResult.Actor);
				HitActor->Jump();
			}
		}
	}
}
void AIBCharacter::OnAssetLoadCompleted()
{
	USkeletalMesh* AssetLoaded = Cast<USkeletalMesh>(AssetStreamingHandle->GetLoadedAsset());
	AssetStreamingHandle.Reset();
	ABCHECK(nullptr != AssetLoaded);
	GetMesh()->SetSkeletalMesh(AssetLoaded);
	
	SetCharacterState(ECharacterState::READY);

}
void AIBCharacter::SetHitEffect(AIBWeapon * NewWeapon)
{
}

void AIBCharacter::TestParameter()
{
	TestInt1 = 1;
	TestFloat1 = 200.0f;
	TestFloat2 = 0.3f;
}

void AIBCharacter::AttackStepAddLambda()
{
	//1Step
	IBAnim->FOnAttackType1_1StepStartCheck.AddLambda([this]() -> void {
		switch (CurrentCombo)
		{
		case 1:
			AttackStepNum = 1;
			break;
		case 2:
			AttackStepNum = 3;
			break;
		case 3:
			AttackStepNum = 4;
			break;
		}
		IsAttackType_1Step = true;
	}); 
	IBAnim->FOnAttackType1_1StepDoneCheck.AddLambda([this]() -> void {
		AttackStepNum = 0;
		IsAttackType_1Step = false;
	});
	
	IBAnim->FOnAttackType1_2StepStartCheck.AddLambda([this]() -> void {
		switch (CurrentCombo)
		{
		case 1:
			AttackStepNum = 2;
			break;
		case 3:
			AttackStepNum = 5;
			break;
		}
		IsAttackType_2Step = true;
	});
	IBAnim->FOnAttackType1_2StepDoneCheck.AddLambda([this]() -> void {
		AttackStepNum = 0;
		IsAttackType_2Step = false;
	});
}

void AIBCharacter::MoveAttackType1()
{
	//SetActorLocation(FMath::VInterpTo(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 300.0f, DeltaTime, 2.0f));
	if (bBasicAttackMontage)
	{
		switch (AttackStepNum)
		{
		case 1:
			if (IsAttackType_1Step)
			{
				SetActorLocation(FMath::VInterpTo(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 400.0f, GetWorld()->GetDeltaSeconds(), 1.25f));
			}
			break;
		case 2:
			if (IsAttackType_2Step)
			{
				SetActorLocation(FMath::VInterpTo(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 350, GetWorld()->GetDeltaSeconds(), 0.25f));
			}
			break;
		case 3:
			if (IsAttackType_1Step)
			{
				SetActorLocation(FMath::VInterpTo(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 400.0f, GetWorld()->GetDeltaSeconds(), 1.25));
			}
			break;
		case 4:
			if (IsAttackType_1Step)
			{
				SetActorLocation(FMath::VInterpTo(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 350, GetWorld()->GetDeltaSeconds(), 1.25));
			}
			break;
		case 5:
			if (IsAttackType_2Step)
			{
				SetActorLocation(FMath::VInterpTo(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 400.0f, GetWorld()->GetDeltaSeconds(), 0.25));
			}
			break;
		}
	}
	if (!bBasicAttackMontage)
	{
		switch (CurrentCombo)
		{
		case 1:
			if (IsAttackType_1Step)
			{
				SetActorLocation(FMath::VInterpTo(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 400.0f, GetWorld()->GetDeltaSeconds(), 0.1f));
			}
			else if (IsAttackType_2Step)
			{
				SetActorLocation(FMath::VInterpTo(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 400.0f, GetWorld()->GetDeltaSeconds(), 0.1f));
			}
			break;
		case 2:
			if (IsAttackType_1Step)
			{
				SetActorLocation(FMath::VInterpTo(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 350, GetWorld()->GetDeltaSeconds(), 0.1f));
			}
			else if (IsAttackType_2Step)
			{
				SetActorLocation(FMath::VInterpTo(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 150, GetWorld()->GetDeltaSeconds(), 0.1f));
			}
			break;
		case 3:
			break;
		case 4:
			if (IsAttackType_1Step)
			{
				SetActorLocation(FMath::VInterpTo(GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 3200, GetWorld()->GetDeltaSeconds(), 0.1f));
			}
			break;
		}
	}
}

void AIBCharacter::InitAttackStep()
{
	AttackStepNum = 0;
	IsAttackType_1Step = false;
	IsAttackType_2Step = false;
}

LSAttackMode AIBCharacter::GetCurrentAttackMode()
{
	return CurrentAttackMode;
}

int32 AIBCharacter::GetCurrntCombo()
{
	return CurrentCombo;
}




void AIBCharacter::InitSkillParticle()
{
	//��ų_1 ��������
	SkillEffect_1 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SKILL_1"));
	SkillEffect_1->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_SKILL_1(TEXT("/Game/InfinityBladeEffects/Effects/FX_Skill_Leap/P_Skill_Leap_Base_Impact.P_Skill_Leap_Base_Impact"));
	if (P_SKILL_1.Succeeded())
	{
		SkillEffect_1->SetTemplate(P_SKILL_1.Object);
		SkillEffect_1->bAutoActivate = false;
		SkillEffect_1->bAbsoluteLocation = true;
		SkillEffect_1->bAbsoluteRotation = true;
	}

	SkillEffect_1_Final = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SKILL_1_Final"));
	SkillEffect_1_Final->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_SKILL_1_Final(TEXT("/Game/InfinityBladeEffects/Effects/FX_Skill_Leap/P_Skill_Leap_Fire_Impact_Suction.P_Skill_Leap_Fire_Impact_Suction"));
	if (P_SKILL_1_Final.Succeeded())
	{
		SkillEffect_1_Final->SetTemplate(P_SKILL_1_Final.Object);
		SkillEffect_1_Final->bAutoActivate = false;
		SkillEffect_1_Final->bAbsoluteLocation = true;
		SkillEffect_1_Final->bAbsoluteRotation = true;
	}

	//��ų_2 ����
	ShieldSkill = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ShieldSkill"));
	ShieldSkill->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_SHIELD(TEXT("/Game/InfinityBladeEffects/Effects/FX_Ability/Defense/P_Shield_Sphere_Buff.P_Shield_Sphere_Buff"));
	if (P_SHIELD.Succeeded())
	{
		ShieldSkill->SetTemplate(P_SHIELD.Object);
		ShieldSkill->bAutoActivate = false;
		ShieldSkill->bAbsoluteLocation = true;
		ShieldSkill->bAbsoluteRotation = true;
		ShieldSkill->SetWorldScale3D(FVector(1.0f, 1.0f, 1.2f));
	}
	
	//��ų_4
	for (int i = 0; i < 8; i++)
	{
		UltimateParticle UP;
		UP.FirstParticle = CreateDefaultSubobject<UParticleSystemComponent>(FName(*FString::Printf(TEXT("FirstParticle_%d"), i)));
		UP.FirstParticle->SetupAttachment(RootComponent);
		UP.FirstParticle->SetTemplate(P_SKILL_1.Object);
		UP.FirstParticle->bAbsoluteLocation = true;
		UP.FirstParticle->bAbsoluteRotation = true;

		UP.SecondParticle = CreateDefaultSubobject<UParticleSystemComponent>(FName(*FString::Printf(TEXT("SecondParticle_%d"), i)));
		UP.SecondParticle->SetupAttachment(RootComponent);
		UP.SecondParticle->SetTemplate(P_SKILL_1_Final.Object);
		UP.SecondParticle->bAbsoluteLocation = true;
		UP.SecondParticle->bAbsoluteRotation = true;


		UP.SkillStartVector = FVector::ZeroVector;
		m_vUSParticleVector.push_back(UP);		
	}

	TestParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TestParticle"));
	TestParticle->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_TEST(TEXT("/Game/InfinityBladeEffects/Effects/FX_Ability/Defense/P_Shield_Sphere_Buff.P_Shield_Sphere_Buff"));
	if (P_TEST.Succeeded())
	{
		TestParticle->SetTemplate(P_TEST.Object);
		TestParticle->bAutoActivate = false;
	}
	TestParticle->bAbsoluteLocation = true;
	TestParticle->bAbsoluteRotation = true;

	TestParticle2 = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TestParticle2"));
	TestParticle2->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> P_TEST2(TEXT("/Game/InfinityBladeEffects/Effects/FX_Ability/Heal/P_Heal_LongDistance_Start.P_Heal_LongDistance_Start"));
	if (P_TEST2.Succeeded())
	{
		TestParticle2->SetTemplate(P_TEST2.Object);
		TestParticle2->bAutoActivate = false;
	}
	TestParticle2->bAbsoluteLocation = true;
	TestParticle2->bAbsoluteRotation = true;
}
void AIBCharacter::InitFirstSkill()
{
	if (ShieldSkill->IsActive())
	{
		ShieldSkill->SetVisibility(false);
	}

	IBAnim->PlayFirstSkillMontage(4);
	IsAttacking = true;
	
}
void AIBCharacter::InitSecondSkill()
{
	if (!bSecondSkillEffect)
	{
		bSecondSkillEffect = true;
		IBAnim->PlayShieldSkillMontage();
		ShieldSkill->Activate(true);
		IsAttacking = true;
	}
}
void AIBCharacter::InitForthSkill()
{
	if (ShieldSkill->IsActive())
	{
		ShieldSkill->SetVisibility(false);
	}

	IBAnim->PlayUltimateSkillMontage();
	SkillStartLocation = GetActorLocation() + FVector(0.0f, 0.0f, -90.0f);
	SkillStartForwardVector = GetActorForwardVector();
	IsAttacking = true;

	SkillStartLocation = GetActorLocation() + FVector(0.0f, 0.0f, -90.0f);
	SkillStartForwardVector = GetActorForwardVector();
	SkillStartForwardLeftVector = GetActorForwardVector() + GetActorRightVector();
	SkillStartForwardRightVector = GetActorForwardVector() - GetActorRightVector();
	SkillStartBackVector = -1.0f*GetActorForwardVector();
	SkillStartBackLeftVector = -1.0f*GetActorForwardVector() + GetActorRightVector();;
	SkillStartBackRightVector = -1.0f*GetActorForwardVector() - GetActorRightVector();;
	SkillStartRightVector = GetActorRightVector();
	SkillStartLeftVector = -1.0f*GetActorRightVector();

}
void AIBCharacter::Skill_3()
{
	TestParticle->SetWorldLocation(GetActorLocation()+GetActorForwardVector()*40.0f);
	TestParticle2->Activate(true);
}
void AIBCharacter::InitGroundBurstSkillParameter()
{
	IsAttacking = false;
	bFirstSkillEffect = false;
	EffectIntervalTime = 0.0f;
	EffectNum = 1;
}
void AIBCharacter::InitShieldSkillParameter()
{
	ShieldSkillActiveTime = 0.0f;
	bSecondSkillEffect = false;
}
void AIBCharacter::InitUltimateSkillParameter()
{
	IsAttacking = false;
	bForthSkillEffect = false;
	bBasicUltimateSkill = false;
	EffectIntervalTime = 0.0f;
	EffectNum = 1;
}
void AIBCharacter::SkillHub(float DeltaTime)
{
	if (bFirstSkillEffect)
	{

		EffectIntervalTime += DeltaTime;
		if (EffectIntervalTime >= 0.05)
		{
			
			EffectIntervalTime = 0.0f;
			
			SkillEffect_1->SetWorldLocation(SkillStartLocation + SkillStartForwardVector * (float)EffectNum*130.0f);
			SkillEffect_1->Activate(true);
			EffectNum++;
			if (EffectNum >= 7)
			{
				SkillEffect_1_Final->SetWorldLocation(SkillStartLocation + SkillStartForwardVector* (float)EffectNum*130.0f);
				SkillEffect_1_Final->Activate(true);
				EffectNum = 1;
				InitGroundBurstSkillParameter();
				
			}
		}

	}

	if (bSecondSkillEffect)
	{
		ShieldSkill->SetWorldLocation(GetActorLocation() + FVector(0.0f, 0.0f, -48.0f));
		ShieldSkillActiveTime += DeltaTime;
		if (ShieldSkillActiveTime >= 5.0f)
		{
			InitShieldSkillParameter();
			ShieldSkill->Activate(false);
			ShieldSkill->Complete();
		}
	}

	if (bForthSkillEffect)
	{
		EffectIntervalTime += DeltaTime;
		
		//�⺻ ��
		if (!bBasicUltimateSkill)
		{
			if (EffectIntervalTime >= 0.05)
			{
				EffectIntervalTime = 0.0f;
				for (auto it = m_vUSParticleVector.begin(); it != m_vUSParticleVector.end(); ++it)
				{
					switch (ParticelNum)
					{
					case 0:
						it->FirstParticle->SetWorldLocation(SkillStartLocation + SkillStartForwardVector * (float)EffectNum*130.0f);
						it->FirstParticle->Activate(true);
						break;
					case 1:
						it->FirstParticle->SetWorldLocation(SkillStartLocation + SkillStartForwardLeftVector * (float)EffectNum*130.0f);
						it->FirstParticle->Activate(true);
						break;
					case 2:
						it->FirstParticle->SetWorldLocation(SkillStartLocation + SkillStartForwardRightVector * (float)EffectNum*130.0f);
						it->FirstParticle->Activate(true);
						break;
					case 3:
						it->FirstParticle->SetWorldLocation(SkillStartLocation + SkillStartBackVector * (float)EffectNum*130.0f);
						it->FirstParticle->Activate(true);
						break;
					case 4:
						it->FirstParticle->SetWorldLocation(SkillStartLocation + SkillStartBackLeftVector * (float)EffectNum*130.0f);
						it->FirstParticle->Activate(true);
						break;
					case 5:
						it->FirstParticle->SetWorldLocation(SkillStartLocation + SkillStartBackRightVector * (float)EffectNum*130.0f);
						it->FirstParticle->Activate(true);
						break;
					case 6:
						it->FirstParticle->SetWorldLocation(SkillStartLocation + SkillStartLeftVector * (float)EffectNum*130.0f);
						it->FirstParticle->Activate(true);
						break;
					case 7:
						it->FirstParticle->SetWorldLocation(SkillStartLocation + SkillStartRightVector * (float)EffectNum*130.0f);
						it->FirstParticle->Activate(true);
						break;
					}
					ParticelNum++;
				}
				ParticelNum = 0;
				EffectNum++;
				if (EffectNum >= 7)
				{
					for (auto it = m_vUSParticleVector.begin(); it != m_vUSParticleVector.end(); ++it)
					{
						switch (ParticelNum)
						{
						case 0:
							it->SecondParticle->SetWorldLocation(SkillStartLocation + SkillStartForwardVector * (float)EffectNum*130.0f);
							it->SecondParticle->Activate(true);
							break;
						case 1:
							it->SecondParticle->SetWorldLocation(SkillStartLocation + SkillStartForwardLeftVector * (float)EffectNum*130.0f);
							it->SecondParticle->Activate(true);
							break;
						case 2:
							it->SecondParticle->SetWorldLocation(SkillStartLocation + SkillStartForwardRightVector * (float)EffectNum*130.0f);
							it->SecondParticle->Activate(true);
							break;
						case 3:
							it->SecondParticle->SetWorldLocation(SkillStartLocation + SkillStartBackVector * (float)EffectNum*130.0f);
							it->SecondParticle->Activate(true);
							break;
						case 4:
							it->SecondParticle->SetWorldLocation(SkillStartLocation + SkillStartBackLeftVector * (float)EffectNum*130.0f);
							it->SecondParticle->Activate(true);
							break;
						case 5:
							it->SecondParticle->SetWorldLocation(SkillStartLocation + SkillStartBackRightVector * (float)EffectNum*130.0f);
							it->SecondParticle->Activate(true);
							break;
						case 6:
							it->SecondParticle->SetWorldLocation(SkillStartLocation + SkillStartLeftVector * (float)EffectNum*130.0f);
							it->SecondParticle->Activate(true);
							break;
						case 7:
							it->SecondParticle->SetWorldLocation(SkillStartLocation + SkillStartRightVector * (float)EffectNum*130.0f);
							it->SecondParticle->Activate(true);
							break;
						}
						ParticelNum++;
					}
					ParticelNum = 0;
					InitUltimateSkillParameter();
				}
			}
		}
		else if (bBasicUltimateSkill)
		{
			//��ȭ ��
			//���� ����
			EffectIntervalTime += DeltaTime;
			if (EffectIntervalTime >= 0.01)
			{

				EffectIntervalTime = 0.0f;

				UNavigationSystem* NavSystem = UNavigationSystem::GetNavigationSystem(GetWorld());
				FNavLocation NextPatrol;
				NavSystem->GetRandomPointInNavigableRadius(GetActorLocation(), 500.0f, NextPatrol);
				SkillEffect_1->SetWorldLocation(NextPatrol.Location);			
				SkillEffect_1->Activate(true);
				EffectNum++;
				if (EffectNum >= 70)
				{
					EffectNum = 7;
					for (auto it = m_vUSParticleVector.begin(); it != m_vUSParticleVector.end(); ++it)
					{
						switch (ParticelNum)
						{
						case 0:
							it->SecondParticle->SetWorldLocation(SkillStartLocation + SkillStartForwardVector * (float)EffectNum*130.0f);
							it->SecondParticle->Activate(true);
							break;
						case 1:
							it->SecondParticle->SetWorldLocation(SkillStartLocation + SkillStartForwardLeftVector * (float)EffectNum*130.0f);
							it->SecondParticle->Activate(true);
							break;
						case 2:
							it->SecondParticle->SetWorldLocation(SkillStartLocation + SkillStartForwardRightVector * (float)EffectNum*130.0f);
							it->SecondParticle->Activate(true);
							break;
						case 3:
							it->SecondParticle->SetWorldLocation(SkillStartLocation + SkillStartBackVector * (float)EffectNum*130.0f);
							it->SecondParticle->Activate(true);
							break;
						case 4:
							it->SecondParticle->SetWorldLocation(SkillStartLocation + SkillStartBackLeftVector * (float)EffectNum*130.0f);
							it->SecondParticle->Activate(true);
							break;
						case 5:
							it->SecondParticle->SetWorldLocation(SkillStartLocation + SkillStartBackRightVector * (float)EffectNum*130.0f);
							it->SecondParticle->Activate(true);
							break;
						case 6:
							it->SecondParticle->SetWorldLocation(SkillStartLocation + SkillStartLeftVector * (float)EffectNum*130.0f);
							it->SecondParticle->Activate(true);
							break;
						case 7:
							it->SecondParticle->SetWorldLocation(SkillStartLocation + SkillStartRightVector * (float)EffectNum*130.0f);
							it->SecondParticle->Activate(true);
							break;
						}
						ParticelNum++;
					}
					InitGroundBurstSkillParameter();
					InitUltimateSkillParameter();
				}
			}
		}
	}
}



