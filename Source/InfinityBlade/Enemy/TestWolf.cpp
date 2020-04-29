// Fill out your copyright notice in the Description page of Project Settings.

#include "TestWolf.h"
#include "IBAnimInstance.h"
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


// Sets default values
ATestWolf::ATestWolf()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//CharacterStat = CreateDefaultSubobject<UIBCharacterStatComponent>(TEXT("CHARACTERSTAT"));
	//HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBARWIDGET"));

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f));
	
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> Wolf(TEXT("/Game/InfinityBladeAdversaries/Enemy/Enemy_Wolf/SK_Enemy_Wolf_Armored.SK_Enemy_Wolf_Armored"));
	if (Wolf.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(Wolf.Object);
	}

	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetCharacterMovement()->MaxWalkSpeed = 400;
	GetCapsuleComponent()->SetCollisionProfileName(TEXT("IBCharacter"));

}

// Called when the game starts or when spawned
void ATestWolf::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestWolf::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATestWolf::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

