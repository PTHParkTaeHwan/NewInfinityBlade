// Fill out your copyright notice in the Description page of Project Settings.

#include "IBCharacterSelecterWidget.h"
#include "IBCharacterSetting.h"
#include "IBGameInstance.h"
#include "EngineUtils.h"
#include "Animation/SkeletalMeshActor.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "IBSaveGame.h"
#include "IBPlayerState.h"


void UIBCharacterSelecterWidget::NextCharacter(bool bForward)
{
	//11, 14
	/*bForward ? CurrentIndex++ : CurrentIndex--;
	if (CurrentIndex == -1) CurrentIndex = MaxIndex - 1;
	if (CurrentIndex == MaxIndex) CurrentIndex = 0;*/

	/*CurrentIndex = 11;
	auto CharacterSetting = GetDefault<UIBCharacterSetting>();
	auto AssetRef = CharacterSetting->CharacterAssets[CurrentIndex];
	ABLOG(Warning, TEXT("CurrentIndex %d"), CurrentIndex);

	auto IBGameInstance = GetWorld()->GetGameInstance<UIBGameInstance>();
	ABCHECK(nullptr != IBGameInstance);
	ABCHECK(TargetComponent.IsValid());


	USkeletalMesh* Asset = IBGameInstance->StreamableManager.LoadSynchronous<USkeletalMesh>(AssetRef);
	if (nullptr != Asset)
	{
		TargetComponent->SetSkeletalMesh(Asset);
	}

	CurrentIndex = 14;
	AssetRef = CharacterSetting->CharacterAssets[CurrentIndex];
	Asset = IBGameInstance->StreamableManager.LoadSynchronous<USkeletalMesh>(AssetRef);
	if (nullptr != Asset)
	{
		TargetComponent2->SetSkeletalMesh(Asset);
	}	*/
}

void UIBCharacterSelecterWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentIndex = 0;
	auto CharacterSetting = GetDefault<UIBCharacterSetting>();
	MaxIndex = CharacterSetting->CharacterAssets.Num();
	int Temp = 0;
	for (TActorIterator<ASkeletalMeshActor> It(GetWorld()); It; ++It)
	{
		if (!TargetComponent.IsValid() && Temp==0)
		{
			TargetComponent = It->GetSkeletalMeshComponent();
			ABLOG(Warning, TEXT("TargetComponent %s"), *TargetComponent->GetName());
			
		}
		if (!TargetComponent2.IsValid() && Temp == 1)
		{
			TargetComponent2 = It->GetSkeletalMeshComponent();
			ABLOG(Warning, TEXT("TargetComponent2 %s"), *TargetComponent2->GetName());
			break;
		}
		Temp++;
	}
	

	CurrentIndex = 11;
	auto AssetRef = CharacterSetting->CharacterAssets[CurrentIndex];
	auto IBGameInstance = GetWorld()->GetGameInstance<UIBGameInstance>();

	USkeletalMesh* Asset = IBGameInstance->StreamableManager.LoadSynchronous<USkeletalMesh>(AssetRef);
	if (nullptr != Asset)
	{
		TargetComponent->SetSkeletalMesh(Asset);
	}

	CurrentIndex = 14;
	AssetRef = CharacterSetting->CharacterAssets[CurrentIndex];
	Asset = IBGameInstance->StreamableManager.LoadSynchronous<USkeletalMesh>(AssetRef);
	if (nullptr != Asset)
	{
		TargetComponent2->SetSkeletalMesh(Asset);
	}

	CurrentIndex = 0;
	
	//for (TActorIterator<ACameraActor> It(GetWorld()); It; ++It)
	//{
	//	
	//	
	//}

	PrevButton = Cast<UButton>(GetWidgetFromName(TEXT("btnPrev")));
	ABCHECK(nullptr != PrevButton);

	NextButton = Cast<UButton>(GetWidgetFromName(TEXT("btnNext")));
	ABCHECK(nullptr != NextButton);

	TextBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("edtPlayerName")));
	ABCHECK(nullptr != TextBox);

	ConfirmButton = Cast<UButton>(GetWidgetFromName(TEXT("btnConfirm")));
	ABCHECK(nullptr != ConfirmButton);

	PrevButton->OnClicked.AddDynamic(this, &UIBCharacterSelecterWidget::OnPrevClicked);
	NextButton->OnClicked.AddDynamic(this, &UIBCharacterSelecterWidget::OnNextClicked);
	ConfirmButton->OnClicked.AddDynamic(this, &UIBCharacterSelecterWidget::OnConfirmClicked);

	NextCharacter(true);

	bFirstClicked = false;
	fLevelOpenBufferTime = 0.0f;
}

void UIBCharacterSelecterWidget::OnPrevClicked()
{
	if (!bFirstClicked)
	{
		bFirstClicked = true;
		CurrentIndex = 14;
		return ;
	}
	if (CurrentIndex == 11)
	{
		CurrentIndex = 14;
	}
	else if (CurrentIndex == 14)
	{
		CurrentIndex = 11;
	}
}

void UIBCharacterSelecterWidget::OnNextClicked()
{
	if (!bFirstClicked)
	{
		bFirstClicked = true;
		CurrentIndex = 11;
		return;
	}

	if (CurrentIndex == 14)
	{
		CurrentIndex = 11;
	}
	else if (CurrentIndex == 11)
	{
		CurrentIndex = 14;
	}
}

void UIBCharacterSelecterWidget::OnConfirmClicked()
{

	FString CharacterName = TextBox->GetText().ToString();
	if (CharacterName.Len() <= 0 || CharacterName.Len() > 20) return;

	//fLevelOpenBufferTime += GetWorld()->GetDeltaSeconds();

	UIBSaveGame* NewPlayerData = NewObject<UIBSaveGame>();
	NewPlayerData->PlayerName = CharacterName;
	NewPlayerData->Level = 1;
	NewPlayerData->Exp = 0;
	NewPlayerData->HighScore = 0;
	NewPlayerData->CharacterIndex = CurrentIndex;
	ABLOG(Warning, TEXT("CurrentIndex %d"), CurrentIndex);

	
	auto IBPlayerState = GetDefault<AIBPlayerState>();
	UGameplayStatics::SaveGameToSlot(NewPlayerData, IBPlayerState->SaveSlotName, 0);
	/*if (UGameplayStatics::SaveGameToSlot(NewPlayerData, IBPlayerState->SaveSlotName, 0))
	{
		UGameplayStatics::OpenLevel(GetWorld(), TEXT("StartRoom"));
	}
	else
	{
		ABLOG(Error, TEXT("SaveGame Error!"));
	}*/

}


