#include "Controllers/LinearPlayerController.h"
#include "Logging/StructuredLog.h"

// Enhanced Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

// Widget 
#include "Components/HUD/MenuContainerWidget.h"

// UI Navigation のキー操作調整用
#include "Framework/Application/NavigationConfig.h"
#include "Framework/Application/SlateApplication.h"

void ALinearPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (MenuContainerWidgetClass)
	{
		MenuContainerWidgetInstance = CreateWidget<UMenuContainerWidget>(this, MenuContainerWidgetClass);
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

	UE_LOGFMT(LogTemp, Warning, "ALinearPlayerController::ToggleMenu()");
	bIsMenuOpen = !bIsMenuOpen;
	// サブシステムの取得
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (bIsMenuOpen)
	{
		// メニューを開く処理
		MenuContainerWidgetInstance->AddToViewport();

		// メニューの操作を キー入力で受け付ける
		FInputModeGameAndUI InputMode; // FInputModeUIOnly とすると、メニューを閉じれないので GameAndUI
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // マウスカーソルがゲームウィンドウ外に出られるようにする
		SetInputMode(InputMode); // PlayerController に設定適用

		bShowMouseCursor = true;

		// メニュー用操作に切り替え
		if (Subsystem && DefaultMappingContext && MenuMappingContext)
		{
			Subsystem->RemoveMappingContext(DefaultMappingContext);
			Subsystem->AddMappingContext(MenuMappingContext, 1);
		}

		// アクションゲームであればポーズをかける
		UGameplayStatics::SetGamePaused(this, true);
	}
	else
	{
		// メニューを閉じる処理
		MenuContainerWidgetInstance->RemoveFromParent();
		MenuContainerWidgetInstance->ResetToMainMenu(); // 次にメニューを開いたとき、初期状態になるようにする

		// 入力モードをゲームプレイのみに戻し、カーソルを非表示
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;

		// 通常操作に切り替え
		if (Subsystem && DefaultMappingContext && MenuMappingContext)
		{
			Subsystem->RemoveMappingContext(MenuMappingContext);
			Subsystem->AddMappingContext(DefaultMappingContext, 0);

		}

		UGameplayStatics::SetGamePaused(this, false);

	}

}