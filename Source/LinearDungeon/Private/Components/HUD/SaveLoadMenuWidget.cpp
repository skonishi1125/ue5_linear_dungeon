#include "Components/HUD/SaveLoadMenuWidget.h"
#include "Logging/StructuredLog.h"

#include "Subsystems/LinearSaveSubsystem.h"
#include "Characters/LinearPlayerCharacter.h"
#include "SaveGames/LinearSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

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

void USaveLoadMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SlotButtons.Empty();
	SlotButtons.Add(SlotButton_0);
	SlotButtons.Add(SlotButton_1);

	SlotTimeStamps.Empty();
	SlotTimeStamps.Add(SlotTimeStamp_0);
	SlotTimeStamps.Add(SlotTimeStamp_1);

	// 画面が出る度、最新状態にする
	RefreshSlotDisplay();

}

// 画面を開いたときやセーブ後に呼び出し、スロットに情報が反映されるようにする
void USaveLoadMenuWidget::RefreshSlotDisplay()
{
	// Subsystemのベース名（"SaveSlot_"）を一時的に定義（本来はSubsystemから取得するか共通化するのが理想）
	const FString BaseSaveSlotName = TEXT("SaveSlot_");

	// スロットの数だけループ処理を行う
	for (int32 i = 0; i < SlotTimeStamps.Num(); ++i)
	{
		if (!SlotTimeStamps[i]) continue;

		FString SaveSlotName = FString::Printf(TEXT("%s%d"), *BaseSaveSlotName, i);

		// セーブデータが存在するかチェック
		if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
		{
			// 存在する場合はデータをロード
			ULinearSaveGame* LoadedGame = Cast<ULinearSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
			if (LoadedGame)
			{
				// 日時を指定フォーマットの文字列に変換してセット
				FString DateString = LoadedGame->SaveDate.ToString(TEXT("%Y.%m.%d %H:%M:%S"));
				SlotTimeStamps[i]->SetText(FText::FromString(DateString));
			}
		}
		else
		{
			// セーブデータが存在しない場合は「NO DATA」と表示
			SlotTimeStamps[i]->SetText(FText::FromString(TEXT("NO DATA")));
		}
	}
}


void USaveLoadMenuWidget::ActivateMenu(ESaveLoadMode InMode)
{
	// Save / Load どちらのモードか、呼ばれた時に指定する
	CurrentMode = InMode;
	RefreshSlotDisplay();

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
		SaveSubsystem->SaveGame(SlotIndex);
		
		// タイムスタンプなど、セーブ後の設定に更新
		RefreshSlotDisplay();
	}
	else if (CurrentMode == ESaveLoadMode::ESL_Load)
	{
		UE_LOGFMT(LogTemp, Warning, "Executing LOAD from Slot: {0}", SlotIndex);
		SaveSubsystem->LoadGame(SlotIndex);
		
		// ロードを実行時、メニューを閉じるようにする処理を親に飛ばすなどする
		OnLoadButtonPressedDelegate.Broadcast();
	}

}
