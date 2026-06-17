#include "Components/HUD/SaveLoadMenuWidget.h"
#include "Logging/StructuredLog.h"

#include "Subsystems/LinearSaveSubsystem.h"
#include "Characters/LinearPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void USaveLoadMenuWidget::ActivateMenu(ESaveLoadMode InMode)
{
	// Save / Load どちらのモードか、呼ばれた時に指定する
	CurrentMode = InMode;

	if (InfoText)
	{
		if (CurrentMode == ESaveLoadMode::ESL_Save)
		{
			InfoText->SetText(FText::FromString("SELECT SAVE SLOTS"));
		}
		else
		{
			InfoText->SetText(FText::FromString("SELECT LOAD SLOTS"));
		}
	}

	// スロットボタンへ UI Focus を移す
	if (SlotButton_0)
	{
		SlotButton_0->SetKeyboardFocus();
	}
}

bool USaveLoadMenuWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (SlotButton_0)
	{
		SlotButton_0->OnClicked.AddDynamic(this, &USaveLoadMenuWidget::OnSlot0Clicked);
	}
	if (SlotButton_1)
	{
		SlotButton_1->OnClicked.AddDynamic(this, &USaveLoadMenuWidget::OnSlot1Clicked);
	}


	return true;
}

void USaveLoadMenuWidget::ExecuteSaveOrLoad(int32 SlotIndex)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	ULinearSaveSubsystem* SaveSubsystem = GI->GetSubsystem<ULinearSaveSubsystem>();
	if (!SaveSubsystem) return;

	// セーブしたい要素の取得
	ALinearPlayerCharacter* PlayerCharacter = Cast<ALinearPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));

	if (CurrentMode == ESaveLoadMode::ESL_Save)
	{
		UE_LOGFMT(LogTemp, Warning, "Executing SAVE to Slot: {0}", SlotIndex);
		SaveSubsystem->SaveGame(PlayerCharacter, SlotIndex);
	}
	else if (CurrentMode == ESaveLoadMode::ESL_Load)
	{
		UE_LOGFMT(LogTemp, Warning, "Executing LOAD from Slot: {0}", SlotIndex);
		SaveSubsystem->LoadGame(PlayerCharacter, SlotIndex);
	}
}
