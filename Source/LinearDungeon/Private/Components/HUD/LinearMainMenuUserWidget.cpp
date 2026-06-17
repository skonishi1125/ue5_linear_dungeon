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
		SaveButton->OnClicked.AddDynamic(this, &ULinearMainMenuUserWidget::OnSaveButtonClicked);
	}

	if (LoadButton)
	{
		LoadButton->OnClicked.AddDynamic(this, &ULinearMainMenuUserWidget::OnLoadButtonClicked);
	}

	return true;
}

void ULinearMainMenuUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (SaveButton)
	{
		SaveButton->SetKeyboardFocus(); // 初期で選ばれているボタンの設定
		UE_LOGFMT(LogTemp, Warning, "ULinearMainMenuUserWidget::NativeConstruct()");
	}
}

void ULinearMainMenuUserWidget::OnSaveButtonClicked()
{
	UE_LOGFMT(LogTemp, Warning, "ULinearMainMenuUserWidget::OnSaveButtonClicked()");
}

void ULinearMainMenuUserWidget::OnLoadButtonClicked()
{
	UE_LOGFMT(LogTemp, Warning, "ULinearMainMenuUserWidget::OnLoadButtonClicked()");
}