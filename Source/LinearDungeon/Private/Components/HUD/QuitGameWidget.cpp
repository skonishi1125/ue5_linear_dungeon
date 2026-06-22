#include "Components/HUD/QuitGameWidget.h"
#include "Logging/StructuredLog.h"

#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

bool UQuitGameWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (NoButton)
	{
		NoButton->OnClicked.AddUniqueDynamic(this, &UQuitGameWidget::OnNoButtonClicked);
	}

	if (YesButton)
	{
		YesButton->OnClicked.AddUniqueDynamic(this, &UQuitGameWidget::OnYesButtonClicked);
	}


	return true;
}



void UQuitGameWidget::NativeConstruct()
{
	// 初期で選ばれているボタンの設定
	if (NoButton)
	{
		NoButton->SetKeyboardFocus();
	}
}

void UQuitGameWidget::OnNoButtonClicked()
{
	OnReturnMainMenuRequestedDelegate.Broadcast();
}

void UQuitGameWidget::OnYesButtonClicked()
{
	UWorld* World = GetWorld();
	APlayerController* SpecificPlayer = GetOwningPlayer();

	if (World && SpecificPlayer)
	{
		// Blueprintの Quit Game ノードと同等の処理を実行
		// 第3引数: EQuitPreference::Quit (終了) または Background (バックグラウンド移行 ※モバイル用)
		// 第4引数: bIgnorePlatformRestrictions (コンソール機などのプラットフォーム制限を無視するか)
		UKismetSystemLibrary::QuitGame(World, SpecificPlayer, EQuitPreference::Quit, false);
	}
}
