#include "Components/HUD/LinearMainMenuUserWidget.h"
#include "Logging/StructuredLog.h"

#include "Components/Button.h"

bool ULinearMainMenuUserWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	// C++ 関数を、ボタンのOnClicked に登録
	if (SaveButton)
	{
		SaveButton->OnClicked.AddUniqueDynamic(this, &ULinearMainMenuUserWidget::OnSaveButtonClicked);
	}

	if (LoadButton)
	{
		LoadButton->OnClicked.AddUniqueDynamic(this, &ULinearMainMenuUserWidget::OnLoadButtonClicked);
	}

	if (SettingsButton)
	{
		SettingsButton->OnClicked.AddUniqueDynamic(this, &ULinearMainMenuUserWidget::OnSettingsButtonClicked);
	}

	if (QuitGameButton)
	{
		QuitGameButton->OnClicked.AddUniqueDynamic(this, &ULinearMainMenuUserWidget::OnQuitGameButtonClicked);
	}

	return true;
}

void ULinearMainMenuUserWidget::FocusFirstButton()
{
	if (SaveButton)
	{
		SaveButton->SetKeyboardFocus(); // 初期で選ばれているボタンの設定
	}
}

void ULinearMainMenuUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FocusFirstButton();
}

void ULinearMainMenuUserWidget::OnSaveButtonClicked()
{
	OnSaveMenuRequestedDelegate.Broadcast();
}

void ULinearMainMenuUserWidget::OnLoadButtonClicked()
{
	OnLoadMenuRequestedDelegate.Broadcast();
}

void ULinearMainMenuUserWidget::OnSettingsButtonClicked()
{
	OnSettingsMenuRequestedDelegate.Broadcast();
}

void ULinearMainMenuUserWidget::OnQuitGameButtonClicked()
{
	OnQuitGameMenuRequestedDelegate.Broadcast();
}
