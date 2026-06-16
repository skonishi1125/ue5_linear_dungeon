#include "Controllers/LinearPlayerController.h"
#include "Logging/StructuredLog.h"


#include "Components/HUD/LinearMainMenuUserWidget.h"
#include "Components/HUD/SaveLoadMenuWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

void ALinearPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuWidgetClass)
	{
		MainMenuWidgetInstance = CreateWidget<ULinearMainMenuUserWidget>(this, MainMenuWidgetClass);
	}
	if (SaveLoadMenuWidgetClass)
	{
		SaveLoadMenuWidgetInstance = CreateWidget<USaveLoadMenuWidget>(this, SaveLoadMenuWidgetClass);
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
	if (!MainMenuWidgetInstance) return;

	UE_LOGFMT(LogTemp, Warning, "ALinearPlayerController::ToggleMenu()");
	bIsMenuOpen = !bIsMenuOpen;
	// サブシステムの取得
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (bIsMenuOpen)
	{
		// メニューを開く処理
		MainMenuWidgetInstance->AddToViewport();

		// 入力モードを UI 対応に変更し、カーソルを表示
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(MainMenuWidgetInstance->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		SetInputMode(InputMode);

		bShowMouseCursor = true;

		// メニュー用操作に切り替え
		if (Subsystem && DefaultMappingContext && MenuMappingContext)
		{
			Subsystem->RemoveMappingContext(DefaultMappingContext);
			Subsystem->AddMappingContext(MenuMappingContext, 1);
			UE_LOGFMT(LogTemp, Warning, "ToggleMenu(): Setting Context");
		}

		// アクションゲームであればポーズをかける
		UGameplayStatics::SetGamePaused(this, true);
		UE_LOGFMT(LogTemp, Warning, "ToggleMenu(): Open!");
	}
	else
	{
		// メニューを閉じる処理
		MainMenuWidgetInstance->RemoveFromParent();

		// 入力モードをゲームプレイのみに戻し、カーソルを非表示
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
		bShowMouseCursor = false;

		// 通常操作に切り替え
		if (Subsystem && DefaultMappingContext && MenuMappingContext)
		{
			Subsystem->RemoveMappingContext(MenuMappingContext);
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
			UE_LOGFMT(LogTemp, Warning, "ToggleMenu(): Removing Context");

		}

		UGameplayStatics::SetGamePaused(this, false);
		UE_LOGFMT(LogTemp, Warning, "ToggleMenu(): Close!");

	}

}