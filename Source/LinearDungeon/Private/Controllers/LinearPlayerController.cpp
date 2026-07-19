#include "Controllers/LinearPlayerController.h"
#include "Logging/StructuredLog.h"

// Enhanced Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

// Widget 
#include "Components/HUD/MenuContainerWidget.h"
#include "Components/HUD/SaveLoadMenuWidget.h"
#include "Components/HUD/LinearGameOverWidget.h"
#include "Components/HUD/LinearRestartWidget.h"

// UI Navigation のキー操作調整用
#include "Framework/Application/NavigationConfig.h"
#include "Framework/Application/SlateApplication.h"

// ToogleMenu
#include "Characters/LinearPlayerCharacter.h"
#include "Characters/CharacterTypes.h"
#include "Subsystems/LinearSequenceSubsystem.h"

// ゲームオーバー
#include "Subsystems/LinearSaveSubsystem.h"
#include "Subsystems/LinearAudioSubsystem.h"

// 音
#include "Sound/SoundBase.h"

void ALinearPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Title -> ゲーム画面と遷移したときの設計
	// InputOnly とし、マウスが画面外に出ないような形をとる
	FInputModeGameOnly InputMode;
	InputMode.SetConsumeCaptureMouseDown(true);
	SetInputMode(InputMode);

	// Widget 生成 
	// Menu と、GameOver の Widget を管理する
	if (MenuContainerWidgetClass)
	{
		MenuContainerWidgetInstance = CreateWidget<UMenuContainerWidget>(this, MenuContainerWidgetClass);
	}
	if (LinearGameOverWidgetClass)
	{
		LinearGameOverWidgetInstance = CreateWidget<ULinearGameOverWidget>(this, LinearGameOverWidgetClass);
	}
	if (LinearRestartWidgetClass)
	{
		LinearRestartWidgetInstance = CreateWidget<ULinearRestartWidget>(this, LinearRestartWidgetClass);
		if (LinearRestartWidgetInstance)
		{
			LinearRestartWidgetInstance->AddToViewport();
			LinearRestartWidgetInstance->PlayFadeOutAnimation();
		}
	}

	// LoadMenu のロードが済んだときに通知されるデリゲートに登録。
	if (MenuContainerWidgetInstance->GetWBPSaveLoadMenu())
	{
		MenuContainerWidgetInstance->GetWBPSaveLoadMenu()->OnLoadButtonPressedDelegate.AddDynamic(this, &ALinearPlayerController::CloseMenu);
	}

	// UMGナビゲーションキーのカスタマイズ
	if (FSlateApplication::IsInitialized())
	{
		TSharedRef<FNavigationConfig> NavConfig = FSlateApplication::Get().GetNavigationConfig();
		NavConfig->bKeyNavigation = true;

		// 既存の設定にW/S/A/Dのルールを追加する
		NavConfig->KeyEventRules.Emplace(EKeys::W, EUINavigation::Up);
		NavConfig->KeyEventRules.Emplace(EKeys::S, EUINavigation::Down);
		NavConfig->KeyEventRules.Emplace(EKeys::A, EUINavigation::Left);
		NavConfig->KeyEventRules.Emplace(EKeys::D, EUINavigation::Right);
	}

}

void ALinearPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// BeginPlay ではなく、OnPossess (憑依時)で Delegate 登録
	// BeginPlay だと実行順序の関係で、デリゲート登録に失敗する可能性があるのでこっちでやる
	if (CachedLinearPlayerCharacter = Cast<ALinearPlayerCharacter>(InPawn))
	{
		CachedLinearPlayerCharacter->OnCharacterDeathDelegate.AddUniqueDynamic(this, &ALinearPlayerController::OnPlayerDied);
		UE_LOGFMT(LogTemp, Warning, "ALinearPlayerController::OnPossess() Add Delegate");
	}
}

void ALinearPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		// メニューのトグルアクションをバインド
		EnhancedInputComponent->BindAction(ToggleMenuAction, ETriggerEvent::Started, this, &ALinearPlayerController::ToggleMenu);
	}
}

void ALinearPlayerController::ToggleMenu()
{

	if (!MenuContainerWidgetInstance) return;


	if (bIsMenuOpen)
	{
		// 何かの拍子にメニューが開きっぱなしになった場合のため、閉じれるようにだけはしておく
		CloseMenu();
	}
	else
	{
		// 会話中の場合弾く
		if (CachedLinearPlayerCharacter->GetCharacterActionState() != EActionState::EAS_Unoccupied) return;

		// Level Sequence 再生中の場合弾く
		if (UGameInstance* GI = GetGameInstance())
		{
			if (ULinearSequenceSubsystem* SequenceSubsystem = GI->GetSubsystem<ULinearSequenceSubsystem>())
			{
				if (SequenceSubsystem->IsPlayingSequence())
				{
					return;
				}
			}
		}


		bIsMenuOpen = true;
		// メニューを開く処理
		MenuContainerWidgetInstance->AddToViewport();

		// メニューの操作を キー入力で受け付ける
		FInputModeGameAndUI InputMode; // FInputModeUIOnly とすると、メニューを閉じれないので GameAndUI
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // マウスカーソルがゲームウィンドウ外に出られるようにする
		SetInputMode(InputMode); // PlayerController に設定適用

		bShowMouseCursor = true;

		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

		// メニュー用操作に切り替え
		if (Subsystem && DefaultMappingContext && MenuMappingContext)
		{
			Subsystem->RemoveMappingContext(DefaultMappingContext);
			Subsystem->AddMappingContext(MenuMappingContext, 1);
		}

		// ポーズ処理 なくてもいいかも。
		//UGameplayStatics::SetGamePaused(this, true);
	}
}

void ALinearPlayerController::CloseMenu()
{
	bIsMenuOpen = false;

	// メニューを閉じる処理
	MenuContainerWidgetInstance->RemoveFromParent();
	MenuContainerWidgetInstance->ResetToMainMenu(); // メニュー初期状態化

	// 入力モードをゲームプレイのみに戻し、カーソルを非表示
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
	bShowMouseCursor = false;

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	// 通常操作に切り替え
	if (Subsystem && DefaultMappingContext && MenuMappingContext)
	{
		Subsystem->RemoveMappingContext(MenuMappingContext);
		Subsystem->AddMappingContext(DefaultMappingContext, 0);

	}

	// ポーズ解除処理
	//UGameplayStatics::SetGamePaused(this, false);
}

void ALinearPlayerController::OnPlayerDied()
{
	UE_LOGFMT(LogTemp, Warning, "OnPlayerDied()");

	// 死亡時、メニューは閉じる
	if (bIsMenuOpen)
	{
		CloseMenu();
	}

	// プレイヤーの操作入力を完全に無効化する
	DisableInput(this);
	bShowMouseCursor = false;

	// BGM 停止
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULinearAudioSubsystem* AudioSubsystem = GI->GetSubsystem<ULinearAudioSubsystem>())
		{
			AudioSubsystem->StopBGM();
		}
	}


	// 死亡カメラやアニメを見せるため、Timer で待機時間を取る
	GetWorldTimerManager().SetTimer(GameOverTimerHandle, this, &ALinearPlayerController::ShowGameOverUI, 3.0f, false);

}

void ALinearPlayerController::ShowGameOverUI()
{
	UE_LOGFMT(LogTemp, Warning, "ShowGameOverUI()");

	// 効果音 UI の音などは、PlaySound2Dを使えばよい
	if (GameOverSound)
	{
		UGameplayStatics::PlaySound2D(this, GameOverSound);
	}

	// GameOver 画面表示
	if (LinearGameOverWidgetInstance)
	{
		LinearGameOverWidgetInstance->AddToViewport();
		LinearGameOverWidgetInstance->PlayFadeInAnimation();

		// UI表示後、テキストを消すまでの待機時間
		GetWorldTimerManager().SetTimer(GameOverTimerHandle, this, &ALinearPlayerController::HideGameOverText, 3.0f, false);
	}
}

void ALinearPlayerController::HideGameOverText()
{
	UE_LOGFMT(LogTemp, Warning, "HideGameOverText()");

	if (LinearGameOverWidgetInstance)
	{
		LinearGameOverWidgetInstance->PlayTextFadeOutAnimation();

		// テキストのフェードアウトが完了するまでの待機時間（例: 1.5秒）
		GetWorldTimerManager().SetTimer(GameOverTimerHandle, this, &ALinearPlayerController::RestartGame, 4.f, false);
	}
}

void ALinearPlayerController::RestartGame()
{
	// SaveSubsystem 非同期ロードだけで対応する場合
	// ※ Die() で調整した PlayerCharacter の全フラグを調整する必要があるのでかなり大変
	//if (UGameInstance* GI = GetGameInstance())
	//{
	//	if (ULinearSaveSubsystem* SaveSubsystem = GI->GetSubsystem<ULinearSaveSubsystem>())
	//	{
	//		SaveSubsystem->LoadGame(0);
	//	}
	//}

	// ゲームオーバーUIを画面から削除
	// → OpenLevel で開き直すので、消さなくてよい
	//if (LinearGameOverWidgetInstance)
	//{
	//	LinearGameOverWidgetInstance->RemoveFromParent();
	//}

	// ロードするデータの予約をする
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULinearSaveSubsystem* SaveSubsystem = GI->GetSubsystem<ULinearSaveSubsystem>())
		{
			SaveSubsystem->SetPendingLoad(0); // 暫定でスロット0を予約
		}
	}



	// Level 名を取得し、開き直す
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this);
	UGameplayStatics::OpenLevel(this, FName(*CurrentLevelName));

}
