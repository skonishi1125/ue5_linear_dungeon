#include "Components/HUD/LinearTitleMenuContainer.h"
#include "Logging/StructuredLog.h"

#include "Components/WidgetSwitcher.h"
#include "Components/HUD/SaveLoadMenuWidget.h"
#include "Components/HUD/LinearTitleMenu.h"
#include "Components/HUD/SettingsMenuWidget.h"
#include "Kismet/GameplayStatics.h"



void ULinearTitleMenuContainer::NativeConstruct()
{
	Super::NativeConstruct();

	if (WBP_LinearTitleMenu)
	{
		WBP_LinearTitleMenu->OnTitleNewGameRequestedDelegate.AddUniqueDynamic(this, &ULinearTitleMenuContainer::HandleTitleNewGameRequested);
		WBP_LinearTitleMenu->OnTitleLoadMenuRequestedDelegate.AddUniqueDynamic(this, &ULinearTitleMenuContainer::HandleTitleLoadMenuRequested);
		WBP_LinearTitleMenu->OnTitleSettingsRequestedDelegate.AddUniqueDynamic(this, &ULinearTitleMenuContainer::HandleTitleSettingsMenuRequested);
		WBP_LinearTitleMenu->OnTitleQuitGameRequestedDelegate.AddUniqueDynamic(this, &ULinearTitleMenuContainer::HandleTitleQuitGameRequested);
	}
}

void ULinearTitleMenuContainer::HandleTitleNewGameRequested()
{
	if (!NextLevel.IsNull())
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, NextLevel);
	}
	else
	{
		UE_LOGFMT(LogTemp, Error, "OpeningLevel is not configured in ULinearTitleMenuContainer.");
	}
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

void ULinearTitleMenuContainer::HandleTitleSettingsMenuRequested()
{
	if (MenuSwitcher && WBP_SettingsMenu)
	{
		MenuSwitcher->SetActiveWidget(WBP_SettingsMenu);
	}
}

void ULinearTitleMenuContainer::HandleCancelInput()
{
	if (!MenuSwitcher) return;

	if (MenuSwitcher->GetActiveWidget() == WBP_SaveLoadMenu)
	{
		MenuSwitcher->SetActiveWidget(WBP_LinearTitleMenu);

		if (WBP_LinearTitleMenu)
		{
			WBP_LinearTitleMenu->FocusFirstButton();
		}
	}
	else if (MenuSwitcher->GetActiveWidget() == WBP_SettingsMenu)
	{
		MenuSwitcher->SetActiveWidget(WBP_LinearTitleMenu);

		if (WBP_LinearTitleMenu)
		{
			WBP_LinearTitleMenu->FocusFirstButton();
		}
	}
}
