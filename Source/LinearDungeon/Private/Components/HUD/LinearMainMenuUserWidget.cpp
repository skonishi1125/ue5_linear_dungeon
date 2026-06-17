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
	UE_LOGFMT(LogTemp, Warning, "ULinearMainMenuUserWidget::OnSaveButtonClicked()");
	OnSaveMenuRequestedDelegate.Broadcast();
}

void ULinearMainMenuUserWidget::OnLoadButtonClicked()
{
	UE_LOGFMT(LogTemp, Warning, "ULinearMainMenuUserWidget::OnLoadButtonClicked()");
	OnLoadMenuRequestedDelegate.Broadcast();
}