#include "Components/HUD/SaveLoadMenuWidget.h"
#include "Logging/StructuredLog.h"

#include "Subsystems/LinearSaveSubsystem.h"
#include "Characters/LinearPlayerCharacter.h"
#include "SaveGames/LinearSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CircularThrobber.h"

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

void USaveLoadMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UGameInstance* GI = GetGameInstance();
	if (GI)
	{
		if (ULinearSaveSubsystem* SaveSubsystem = GI->GetSubsystem<ULinearSaveSubsystem>())
		{
			SaveSubsystem->OnSaveLoadCompleted.RemoveDynamic(this, &USaveLoadMenuWidget::OnSaveLoadCompleted);
			SaveSubsystem->OnSaveLoadCompleted.AddDynamic(this, &USaveLoadMenuWidget::OnSaveLoadCompleted);
		}
	}
}

void USaveLoadMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// NativeInitialized に移行しても良いかも。
	SlotButtons.Empty();
	SlotButtons.Add(SlotButton_0);
	SlotButtons.Add(SlotButton_1);

	SlotTimeStamps.Empty();
	SlotTimeStamps.Add(SlotTimeStamp_0);
	SlotTimeStamps.Add(SlotTimeStamp_1);

	Overlay_Processing->SetVisibility(ESlateVisibility::Hidden);
	Text_ProcessingInfo->SetVisibility(ESlateVisibility::Hidden);
	Circular_ProcessingInfo->SetVisibility(ESlateVisibility::Hidden);

	// 画面が出る度、最新状態にする
	RefreshSlotDisplay();
}

void USaveLoadMenuWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// Pause 中は TimerHandle のような、待ってから関数を実行する処理が使えない
	// なので、NativeTick を使って 1 秒間の経過を観察して、関数を実行。関連する情報もリセット。
	if (bIsProcessingWait)
	{
		ProcessingWaitTime += InDeltaTime;

		if (ProcessingWaitTime >= 1.0f)
		{
			bIsProcessingWait = false;
			ProcessingWaitTime = 0.0f;
			FinishProcessingUI();
		}
	}

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
			// セーブデータが存在しない場合
			// TimeStamp にメッセージと、ロード画面ならボタンを押せなくする
			SlotTimeStamps[i]->SetText(FText::FromString(TEXT("NO DATA")));
			if (CurrentMode == ESaveLoadMode::ESL_Load)
			{
				UE_LOGFMT(LogTemp, Warning, "ESaveLoadMode::ESL_Load Mode");
				SlotButtons[i]->SetIsEnabled(false);
			}
			else
			{
				// ロード -> セーブと開いたとき、有効化されるようにしておく
				SlotButtons[i]->SetIsEnabled(true);
			}
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

	// 実行時点で、セーブ用の Widget を出す為の処理を行う
	if (Overlay_Processing)
	{
		Overlay_Processing->SetVisibility(ESlateVisibility::Visible);
	}
	if (Text_ProcessingInfo)
	{
		Text_ProcessingInfo->SetVisibility(ESlateVisibility::Visible);
		FString InfoStr = (CurrentMode == ESaveLoadMode::ESL_Save) ? TEXT("SAVING...") : TEXT("LOADING...");
		Text_ProcessingInfo->SetText(FText::FromString(InfoStr));
	}
	if (Circular_ProcessingInfo)
	{
		Circular_ProcessingInfo->SetVisibility(ESlateVisibility::Visible);
	}
	for (UButton* Btn : SlotButtons)
	{
		// 全てのボタンを無効化して、連打を防止する
		if (Btn)
		{
			Btn->SetIsEnabled(false);
		}
	}

	ULinearSaveSubsystem* SaveSubsystem = GI->GetSubsystem<ULinearSaveSubsystem>();
	if (!SaveSubsystem) return;

	if (CurrentMode == ESaveLoadMode::ESL_Save)
	{
		SaveSubsystem->SaveGame(SlotIndex);
	}
	else if (CurrentMode == ESaveLoadMode::ESL_Load)
	{
		SaveSubsystem->LoadGame(SlotIndex);
	}

}

// Subsystem の SaveGame / LoadGame を受け取った後、UI を閉じる処理をタイマーで実行
// (爆速のセーブ / ロードでも、一定のロードがかかるような形にする)
void USaveLoadMenuWidget::OnSaveLoadCompleted()
{

	// Pause 中は TimerHandle を使った処理ができないので、Tick でやる
	bIsProcessingWait = true;
	ProcessingWaitTime = 0.0f;

	//UWorld* World = GetWorld();
	//if (World)
	//{
	//	// セーブロードは最低 1.0f かかるようにする
	//	World->GetTimerManager().SetTimer(
	//		ProcessingTimerHandle,this,	&USaveLoadMenuWidget::FinishProcessingUI,1.0f,false // ループさせない
	//	);
	//}
}

// 通知完了後、1秒後に呼ばれる関数
void USaveLoadMenuWidget::FinishProcessingUI()
{
	UE_LOGFMT(LogTemp, Warning, "USaveLoadMenuWidget::FinishProcessingUI()");

	// Overlay 無効化、ボタン最有効化
	if (Overlay_Processing)
	{
		Overlay_Processing->SetVisibility(ESlateVisibility::Hidden);
	}
	if (Text_ProcessingInfo)
	{
		Text_ProcessingInfo->SetVisibility(ESlateVisibility::Hidden);
	}
	if (Circular_ProcessingInfo)
	{
		Circular_ProcessingInfo->SetVisibility(ESlateVisibility::Hidden);
	}
	for (UButton* Btn : SlotButtons)
	{
		if (Btn) Btn->SetIsEnabled(true);
	}

	// 処理に応じた元の後処理を実行
	if (CurrentMode == ESaveLoadMode::ESL_Save)
	{
		RefreshSlotDisplay();

		// 再びスロットにフォーカスを戻す
		if (SlotButton_0)
		{
			SlotButton_0->SetKeyboardFocus();
		}
		UE_LOGFMT(LogTemp, Warning, "ESL_Save");

	}
	else if (CurrentMode == ESaveLoadMode::ESL_Load)
	{
		OnLoadButtonPressedDelegate.Broadcast();
		UE_LOGFMT(LogTemp, Warning, "ESL_Load");

	}
}

