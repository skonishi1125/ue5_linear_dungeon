#include "Components/HUD/LinearMainMenuUserWidget.h"
#include "Logging/StructuredLog.h"

#include "Components/Button.h"

bool ULinearMainMenuUserWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	// C++ ŠÖ”‚ðAƒ{ƒ^ƒ“‚ÌOnClicked ‚É“o˜^
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

void ULinearMainMenuUserWidget::OnSaveButtonClicked()
{
	UE_LOGFMT(LogTemp, Warning, "ULinearMainMenuUserWidget::OnSaveButtonClicked()");
}

void ULinearMainMenuUserWidget::OnLoadButtonClicked()
{
	UE_LOGFMT(LogTemp, Warning, "ULinearMainMenuUserWidget::OnLoadButtonClicked()");
}