#include "Controllers/LinearTitlePlayerController.h"
#include "Logging/StructuredLog.h"

// UI Navigation のキー操作調整用
#include "Framework/Application/NavigationConfig.h"
#include "Framework/Application/SlateApplication.h"

// Enhanced Input
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

// CloseTitleSubMenu と HUD, Container との紐づけ
#include "Components/HUD/LinearTitleHUD.h"
#include "Components/HUD/LinearTitleMenuContainer.h"

void ALinearTitlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// UI Only ではなく、GameAndUI に設定して、
	// LinearPlayerController の Enhanced Input に移った時も効くようにする
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	// マウスカーソルを表示
	bShowMouseCursor = true;

	// Enhanced Input 初期設定
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		if (TitleMenuMappingContext)
		{
			// 優先度 0 でコンテキストを追加
			Subsystem->AddMappingContext(TitleMenuMappingContext, 0);
		}
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

void ALinearTitlePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(CloseTitleSubMenuAction, ETriggerEvent::Started, this, &ALinearTitlePlayerController::CloseTitleSubMenu);
	}
	UE_LOGFMT(LogTemp, Warning, "ALinearTitlePlayerController::SetupInputComponent()");

}

void ALinearTitlePlayerController::CloseTitleSubMenu()
{
	UE_LOGFMT(LogTemp, Warning, "ALinearTitlePlayerController::CloseTitleSubMenu()");
	
	if (ALinearTitleHUD* TitleHUD = Cast<ALinearTitleHUD>(GetHUD()))
	{
		// HUD が保持している Container の、サブメニューを閉じる処理を呼ぶ
		if (ULinearTitleMenuContainer* Container = TitleHUD->GetMenuContainer())
		{
			Container->HandleCancelInput();
		}
	}

}
