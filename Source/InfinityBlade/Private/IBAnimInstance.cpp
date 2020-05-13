// Fill out your copyright notice in the Description page of Project Settings.

#include "IBAnimInstance.h"
#include "IBCharacter.h"


UIBAnimInstance::UIBAnimInstance()
{
	CurrentPawnSpeed = 0.0f;
	IsInAir = false;
	IsDead = false;
	IsDefense = false;
	static ConstructorHelpers::FObjectFinder<UAnimMontage> ATTACK_MONTAGE(TEXT("/Game/Book/Animations/SK_Mannequin_Skeleton_Montage.SK_Mannequin_Skeleton_Montage"));
	if (ATTACK_MONTAGE.Succeeded())
	{
		AttackMontage = ATTACK_MONTAGE.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<UAnimMontage> LSATTACK_MONTAGE(TEXT("/Game/Book/Animations/Attack_Montage/LongSword_AttackMontage_Type1.LongSword_AttackMontage_Type1"));
	if (LSATTACK_MONTAGE.Succeeded())
	{
		LSAttackMontage = LSATTACK_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> LSBASICATTACK_MONTAGE(TEXT("/Game/Book/Animations/Attack_Montage/LongSword_AttackMontage_BasicAttack.LongSword_AttackMontage_BasicAttack"));
	if (LSBASICATTACK_MONTAGE.Succeeded())
	{
		LSBasicAttackMontage = LSBASICATTACK_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> FIRST_MONTAGE(TEXT("/Game/Book/Animations/Attack_Montage/SK_Mannequin_Skeleton_FirstSkillMontage.SK_Mannequin_Skeleton_FirstSkillMontage"));
	if (FIRST_MONTAGE.Succeeded())
	{
		ClawMontage = FIRST_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> SHIELD_MONTAGE(TEXT("/Game/Book/Animations/Attack_Montage/SK_Mannequin_Skeleton_ShieldSkillMontage.SK_Mannequin_Skeleton_ShieldSkillMontage"));
	if (SHIELD_MONTAGE.Succeeded())
	{
		ShieldMontage = SHIELD_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> ULTIMATE_MONTAGE(TEXT("/Game/Book/Animations/Attack_Montage/SK_Mannequin_Skeleton_UltimateSkillMontage.SK_Mannequin_Skeleton_UltimateSkillMontage"));
	if (ULTIMATE_MONTAGE.Succeeded())
	{
		UltimateSkill = ULTIMATE_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Hit_MONTAGE(TEXT("/Game/Book/Animations/Hit_Montage/SK_Mannequin_Skeleton_HitMontage.SK_Mannequin_Skeleton_HitMontage"));
	if (Hit_MONTAGE.Succeeded())
	{
		HitMontage = Hit_MONTAGE.Object;
	}

	static ConstructorHelpers::FObjectFinder<UAnimMontage> Dodge_MONTAGE(TEXT("/Game/Book/Animations/Dodge_Montage/SK_Mannequin_Skeleton_DodgeMontage.SK_Mannequin_Skeleton_DodgeMontage"));
	if (Dodge_MONTAGE.Succeeded())
	{
		DodgeMontage = Dodge_MONTAGE.Object;
	}

	TestParameter();

}
//================================
//          PlayMontage         //
//================================
void UIBAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	auto Pawn = TryGetPawnOwner();
	
	if (!::IsValid(Pawn)) return;

	if (!IsDead)
	{
		CurrentPawnSpeed = Pawn->GetVelocity().Size();
		//Pawn->GetMovementComponent()->IsFalling();
		auto Character = Cast<ACharacter>(Pawn);
		if (Character)
		{
			IsInAir = Character->GetMovementComponent()->IsFalling();
		}
		auto testCh = Cast<AIBCharacter>(Pawn);
		if (testCh)
		{			
			IsRun = testCh->GetIsRun();
			IsDefense = testCh->GetIsDefense();
		}
	}
}
void UIBAnimInstance::PlayAttackMontage()
{
	ABCHECK(!IsDead);
	switch (CurrentAttackMontageType)
	{
	case WeaponType::BASICSWORD:
		Montage_Play(AttackMontage, 1.0f);
		break;
	case WeaponType::LONGSWORD:
		Montage_Play(LSAttackMontage, 1.2f);
		break;
	case WeaponType::MIDDLESWORD:
		break;
	}
}
void UIBAnimInstance::StopAttackMontage()
{
	switch (CurrentAttackMontageType)
	{
	case WeaponType::LONGSWORD:
		Montage_Stop(0.1f, LSAttackMontage);
		break;
	case WeaponType::MIDDLESWORD:
		break;
	}
}
void UIBAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	ABCHECK(!IsDead);
	auto Pawn = TryGetPawnOwner();
	auto Player = Cast<AIBCharacter>(Pawn);

	switch (CurrentAttackMontageType)
	{
	case WeaponType::BASICSWORD:
		Montage_JumpToSection(GetAttackMontageSectionName(NewSection), AttackMontage);
		break;
	case WeaponType::LONGSWORD:
		if (Player->GetCurrentAttackMode() == LSAttackMode::BASIC)
		{
			Montage_JumpToSection(GetAttackMontageSectionName(NewSection), LSBasicAttackMontage);
		}
		else if (Player->GetCurrentAttackMode() == LSAttackMode::COMBO)
		{
			Montage_JumpToSection(GetAttackMontageSectionName(NewSection), LSAttackMontage);
		}
		break;
	case WeaponType::MIDDLESWORD:
		break;
	}
}
void UIBAnimInstance::PlayBasicAttackNontage()
{
	Montage_Play(LSBasicAttackMontage, 1.4f);
}
void UIBAnimInstance::StopBasicAttackMontage()
{
	Montage_Stop(0.2f, LSBasicAttackMontage);
}
void UIBAnimInstance::JumpToBasicAttackMontageSection(int32 NewSection)
{
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection), LSBasicAttackMontage);
}
void UIBAnimInstance::AnimNotify_AttackHitCheck()
{
	switch (CurrentAttackMontageType)
	{
	case WeaponType::BASICSWORD:
		OnAttackHitCheck.Broadcast();
		break;
	case WeaponType::LONGSWORD:
		OnAttackHitCheck.Broadcast();
		break;
	case WeaponType::MIDDLESWORD:
		break;
	}
}
void UIBAnimInstance::AnimNotify_NextAttackCheck()
{
	switch (CurrentAttackMontageType)
	{
	case WeaponType::BASICSWORD:
		OnNextAttackCheck.Broadcast();
		break;
	case WeaponType::LONGSWORD:
		OnNextAttackCheck.Broadcast();
		break;
	case WeaponType::MIDDLESWORD:
		break;
	}
}
FName UIBAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	ABCHECK(FMath::IsWithinInclusive<int32>(Section, 1, 4), NAME_None);

	auto Pawn = TryGetPawnOwner();
	auto Player = Cast<AIBCharacter>(Pawn);

	switch (CurrentAttackMontageType)
	{
	case WeaponType::BASICSWORD:
		return FName(*FString::Printf(TEXT("Attack%d"), Section));
		break;

	case WeaponType::LONGSWORD:
		if (Player->GetCurrentAttackMode() == LSAttackMode::BASIC)
		{
			return FName(*FString::Printf(TEXT("Attack%d"), Section));
		}
		else if (Player->GetCurrentAttackMode() == LSAttackMode::COMBO)
		{
			return FName(*FString::Printf(TEXT("AttackType%d"), Section));
		}
		break;
	case WeaponType::MIDDLESWORD:
		return FName(*FString::Printf(TEXT("Attack%d"), Section));
		break;
	}
	return FName(*FString::Printf(TEXT("AttackType%d"), Section));
}
void UIBAnimInstance::SetAttackMontageType(WeaponType NewType)
{
	CurrentAttackMontageType = NewType;
}
void UIBAnimInstance::PlayFirstSkillMontage(int32 SectionNum)
{
	Montage_Play(AttackMontage, 1.0f);
	Montage_JumpToSection(FName(*FString::Printf(TEXT("Attack4"))), AttackMontage);	
}
void UIBAnimInstance::PlayClawSkillMontage()
{
	Montage_Play(ClawMontage, 1.2f);
}
void UIBAnimInstance::PlayShieldSkillMontage()
{
	Montage_Play(ShieldMontage, 1.2f);
}
void UIBAnimInstance::PlayUltimateSkillMontage()
{
	Montage_Play(UltimateSkill, 1.0f);
}
void UIBAnimInstance::PlayHitMontage(int32 SectionNum)
{
	Montage_Play(HitMontage, 1.2f);
	Montage_JumpToSection(GetHitMontageSectionName(SectionNum), HitMontage);
}
FName UIBAnimInstance::GetHitMontageSectionName(int32 Section)
{
	return FName(*FString::Printf(TEXT("Hit%d"), Section));
}
bool UIBAnimInstance::GetIsPlayHitMontage()
{
	return Montage_IsPlaying(HitMontage);
}
void UIBAnimInstance::PlayDodgeMontage(int32 SectionNum)
{
	Montage_Play(DodgeMontage, 2.0f);
	Montage_JumpToSection(GetDodgeMontageSectionName(SectionNum), DodgeMontage);
}
FName UIBAnimInstance::GetDodgeMontageSectionName(int32 Section)
{
	return FName(*FString::Printf(TEXT("Dodge%d"), Section));
}
bool UIBAnimInstance::GetIsDodgeMontage()
{
	return Montage_IsPlaying(DodgeMontage);
}
//================================
//          AnimNotify          //
//================================
void UIBAnimInstance::AnimNotify_AttackType1_1StepStart()
{
	FOnAttackType1_1StepStartCheck.Broadcast();
}
void UIBAnimInstance::AnimNotify_AttackType1_1StepDone()
{
	FOnAttackType1_1StepDoneCheck.Broadcast();
}
void UIBAnimInstance::AnimNotify_AttackType1_2StepStart()
{
	FOnAttackType1_2StepStartCheck.Broadcast();
}
void UIBAnimInstance::AnimNotify_AttackType1_2StepDone()
{
	FOnAttackType1_2StepDoneCheck.Broadcast();
}
void UIBAnimInstance::AnimNotify_FirstSkillStart()
{
	FOnFirstSkillStartCheck.Broadcast();
}
void UIBAnimInstance::AnimNotify_ShieldStart()
{
	FOnShieldSkillStart.Broadcast();
}
void UIBAnimInstance::AnimNotify_ShieldSkillDone()
{
	FOnSecondSkillDoneCheck.Broadcast();
}
void UIBAnimInstance::AnimNotify_UltimateSkillStart()
{
	FOnForthSkillStartCheck.Broadcast();
}
void UIBAnimInstance::AnimNotify_FirstSkillStepStart()
{
	ABLOG(Warning, TEXT("AnimNotify_FirstSkillStepStart"));
	FOnFirstSkillStepCheck.Broadcast();
}
void UIBAnimInstance::AnimNotify_FirstSkillStepDone()
{
	FOnFirstSkillStepCheck.Broadcast();
}
void UIBAnimInstance::AnimNotify_HitMotionDone()
{
	Montage_Stop(0.3f, HitMontage);
	FOnHitMotionDoneCheck.Broadcast();
	ABLOG(Warning, TEXT("AnimNotify_HitMotionDone"));
}
void UIBAnimInstance::AnimNotify_DodgeMotionDone()
{
	FOnDodgeMotionDoneCheck.Broadcast();
}

void UIBAnimInstance::TestParameter()
{
	TestFloat1 = 1.0f;
	TestFloat2 = 1.0f;
	TestFloat3 = 1.0f;
	TestInt1 = 0;
}