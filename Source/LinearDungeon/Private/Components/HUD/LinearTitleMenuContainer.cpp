#include "Components/HUD/LinearTitleMenuContainer.h"
#include "Logging/StructuredLog.h"

#include "Components/WidgetSwitcher.h"
#include "Components/HUD/SaveLoadMenuWidget.h"
#include "Components/HUD/LinearTitleMenu.h"


void ULinearTitleMenuContainer::NativeConstruct()
{
	if (WBP_LinearTitleMenu)
	{
		WBP_LinearTitleMenu->OnTitleNewGameRequestedDelegate.AddUniqueDynamic(this, &ULinearTitleMenuContainer::HandleTitleNewGameRequested);
		WBP_LinearTitleMenu->OnTitleLoadMenuRequestedDelegate.AddUniqueDynamic(this, &ULinearTitleMenuContainer::HandleTitleLoadMenuRequested);
		WBP_LinearTitleMenu->OnTitleQuitGameRequestedDelegate.AddUniqueDynamic(this, &ULinearTitleMenuContainer::HandleTitleQuitGameRequested);
	}
}

void ULinearTitleMenuContainer::HandleTitleNewGameRequested()
{
	UE_LOGFMT(LogTemp, Warning, "ULinearTitleMenuContainer::HandleTitleNewGameRequested()");
}

void ULinearTitleMenuContainer::HandleTitleLoadMenuRequested()
{
	if (MenuSwitcher && WBP_SaveLoadMenu)
	{
		MenuSwitcher->SetActiveWidget(WBP_SaveLoadMenu);
		WBP_SaveLoadMenu->ActivateMenu(ESaveLoadMode::ESL_Load);
	}
}

void ULinearTitleMenuContainer::HandleTitleQuitGameRequested()
{
	UE_LOGFMT(LogTemp, Warning, "ULinearTitleMenuContainer::HandleTitleQuitGameRequested()");
}
