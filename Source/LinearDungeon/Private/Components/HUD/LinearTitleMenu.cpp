#include "Components/HUD/LinearTitleMenu.h"
#include "Logging/StructuredLog.h"

#include "Components/Button.h"

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

void ULinearTitleMenu::OnQuitGameButtonClicked()
{
	OnTitleQuitGameRequestedDelegate.Broadcast();
}


