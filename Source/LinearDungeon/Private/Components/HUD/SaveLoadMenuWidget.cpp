#include "Components/HUD/SaveLoadMenuWidget.h"
#include "Logging/StructuredLog.h"

#include "Subsystems/LinearSaveSubsystem.h"
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
	if (SlotButton_2)
	{
		SlotButton_2->OnClicked.AddDynamic(this, &USaveLoadMenuWidget::OnSlot2Clicked);
	}
	if (SlotButton_3)
	{
		SlotButton_3->OnClicked.AddDynamic(this, &USaveLoadMenuWidget::OnSlot3Clicked);
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
			SaveSubsystem->OnSaveCompleted.RemoveDynamic(this, &USaveLoadMenuWidget::OnSaveCompleted);
			SaveSubsystem->OnSaveCompleted.AddDynamic(this, &USaveLoadMenuWidget::OnSaveCompleted);
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
	SlotButtons.Add(SlotButton_2);
	SlotButtons.Add(SlotButton_3);

	SlotTimeStamps.Empty();
	SlotTimeStamps.Add(SlotTimeStamp_0);
	SlotTimeStamps.Add(SlotTimeStamp_1);
	SlotTimeStamps.Add(SlotTimeStamp_2);
	SlotTimeStamps.Add(SlotTimeStamp_3);

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
		bool bHasData = UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0);

		// セーブデータが存在するかチェック
		if (bHasData)
		{
			// 存在する場合はデータをロードし、そのデータからタイムスタンプを取り出して Text に記載
			ULinearSaveGame* LoadedGame = Cast<ULinearSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
			if (LoadedGame)
			{
				FString DateString = LoadedGame->SaveDate.ToString(TEXT("%Y.%m.%d %H:%M:%S"));
				SlotTimeStamps[i]->SetText(FText::FromString(DateString));
			}
		}
		else
		{
			SlotTimeStamps[i]->SetText(FText::FromString(TEXT("NO DATA")));
		}

		// ボタン状態の更新
		if (CurrentMode == ESaveLoadMode::ESL_Save)
		{
			if (i == 0)
			{
				// セーブモード時、スロット0 (AutoSave) は常に無効（オートセーブ用なので上書きできないようにしておく）
				SlotButtons[i]->SetIsEnabled(false);
			}
			else
			{
				// その他のスロットはセーブ可能なので常に有効化
				SlotButtons[i]->SetIsEnabled(true);
			}
		}
		else if (CurrentMode == ESaveLoadMode::ESL_Load)
		{
			// ロードモード時は、データが存在するスロットのみ有効化
			SlotButtons[i]->SetIsEnabled(bHasData);
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

	RefreshSlotDisplay();

	// フォーカスの設定
	if (CurrentMode == ESaveLoadMode::ESL_Save)
	{
		if (SlotButton_1) SlotButton_1->SetKeyboardFocus();
	}
	else
	{
		// ロードモード時、Slot0 が有効ならフォーカスを当てる
		if (SlotButton_0 && SlotButton_0->GetIsEnabled())
		{
			SlotButton_0->SetKeyboardFocus();
		}
	}
}

// Load の流れ
// スロットを押すと、LOADING...の Widget を出しつつ、ボタンを無効化して連打防止する。
// bIsProcessingWait という待機フラグを ON にしておいて Tick を起動させる
// Tick で指定秒数経ったら、FinishProcessingUI() が走る

void USaveLoadMenuWidget::ExecuteSaveOrLoad(int32 SlotIndex)
{
	UGameInstance* GI = GetGameInstance();
	if (!GI) return;

	SelectedSlotIndex = SlotIndex;

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
		// ロード用のスロットが押された時の挙動
		// SaveSubsystem に、ロードするスロットの Index を登録しておく
		SaveSubsystem->SetPendingLoad(SlotIndex);
		bIsProcessingWait = true;
		ProcessingWaitTime = 0.0f;
	}

}

// Subsystem の SaveGame  を受け取った後、UI を閉じる処理をタイマーで実行
// (爆速のセーブ時に、一定のロードがかかるような形にする)
void USaveLoadMenuWidget::OnSaveCompleted()
{
	// 1. 現在がセーブモードでない場合（ロード画面を開いている時など）は処理を無視する
	if (CurrentMode != ESaveLoadMode::ESL_Save)
	{
		return;
	}

	// 2. プレイヤーがスロットを押して処理中（UIが表示されている）状態であるか確認する
	// ※これがないと、セーブ画面を開いて何も押していない時にオートセーブが完了した場合にも反応する
	if (Overlay_Processing && !Overlay_Processing->IsVisible())
	{
		return;
	}

	// Pause 中は TimerHandle を使った処理ができないので、Tick でやる
	bIsProcessingWait = true;
	ProcessingWaitTime = 0.0f;
}

// 通知完了後、1秒後に呼ばれる関数
// Load 時、OpenLevel を実行して自身の Map に対してロードを行う
// OpenLevel したときに GameMode の BeginPlay が反応し、↑で設定した SetPendingLoad のデータが読まれる
void USaveLoadMenuWidget::FinishProcessingUI()
{
	UE_LOGFMT(LogTemp, Warning, "USaveLoadMenuWidget::FinishProcessingUI()");

	// Overlay 無効化、ボタン再有効化
	if (Overlay_Processing) Overlay_Processing->SetVisibility(ESlateVisibility::Hidden);
	if (Text_ProcessingInfo) Text_ProcessingInfo->SetVisibility(ESlateVisibility::Hidden);
	if (Circular_ProcessingInfo) Circular_ProcessingInfo->SetVisibility(ESlateVisibility::Hidden);

	// 処理に応じた元の後処理を実行
	if (CurrentMode == ESaveLoadMode::ESL_Save)
	{
		RefreshSlotDisplay();
		if (SlotButtons.IsValidIndex(SelectedSlotIndex) && SlotButtons[SelectedSlotIndex])
		{
			SlotButtons[SelectedSlotIndex]->SetKeyboardFocus();
		}

	}
	else if (CurrentMode == ESaveLoadMode::ESL_Load)
	{
		OnLoadButtonPressedDelegate.Broadcast();

		const FString BaseSaveSlotName = TEXT("SaveSlot_");
		FString SaveSlotName = FString::Printf(TEXT("%s%d"), *BaseSaveSlotName, SelectedSlotIndex);

		// フォールバック用
		FString LevelNameToLoad = UGameplayStatics::GetCurrentLevelName(this);

		// スロットからセーブされたデータの名前を読み取り、その名前で Level を開く
		if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
		{
			ULinearSaveGame* LoadedGame = Cast<ULinearSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
			if (LoadedGame && !LoadedGame->SavedLevelName.IsEmpty())
			{
				LevelNameToLoad = LoadedGame->SavedLevelName;
			}
		}

		UGameplayStatics::OpenLevel(this, FName(*LevelNameToLoad));
	}
}

