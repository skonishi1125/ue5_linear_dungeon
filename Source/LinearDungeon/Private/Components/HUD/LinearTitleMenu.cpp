#include "Components/HUD/LinearTitleMenu.h"
#include "Logging/StructuredLog.h"

#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"

bool ULinearTitleMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (NewGameButton)
	{
		NewGameButton->OnClicked.AddUniqueDynamic(this, &ULinearTitleMenu::OnNewGameButtonClicked);
	}
	if (LoadButton)
	{
		LoadButton->OnClicked.AddUniqueDynamic(this, &ULinearTitleMenu::OnLoadButtonClicked);
	}
	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddUniqueDynamic(this, &ULinearTitleMenu::OnSettingsButtonClicked);
	}
	if (QuitGameButton)
	{
		QuitGameButton->OnClicked.AddUniqueDynamic(this, &ULinearTitleMenu::OnQuitGameButtonClicked);
	}

	return true;
}

void ULinearTitleMenu::NativeConstruct()
{
	Super::NativeConstruct();
	FocusFirstButton();
}

void ULinearTitleMenu::FocusFirstButton()
{
	if (NewGameButton)
	{
		NewGameButton->SetKeyboardFocus();
	}
}

void ULinearTitleMenu::OnNewGameButtonClicked()
{
	OnTitleNewGameRequestedDelegate.Broadcast();
}

void ULinearTitleMenu::OnLoadButtonClicked()
{
	OnTitleLoadMenuRequestedDelegate.Broadcast();

}

void ULinearTitleMenu::OnSettingsButtonClicked()
{
	OnTitleSettingsRequestedDelegate.Broadcast();
}

void ULinearTitleMenu::OnQuitGameButtonClicked()
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
	//OnTitleQuitGameRequestedDelegate.Broadcast();
}


