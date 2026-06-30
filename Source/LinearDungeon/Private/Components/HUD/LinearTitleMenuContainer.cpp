#include "Components/HUD/LinearTitleMenuContainer.h"
#include "Logging/StructuredLog.h"

#include "Components/WidgetSwitcher.h"
#include "Components/HUD/SaveLoadMenuWidget.h"
#include "Components/HUD/LinearTitleMenu.h"


void ULinearTitleMenuContainer::NativeConstruct()
{
	Super::NativeConstruct();

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

void ULinearTitleMenuContainer::HandleCancelInput()
{
	UE_LOGFMT(LogTemp, Warning, "ULinearTitleMenuContainer::HandleCancelInput()");
	if (!MenuSwitcher) return;

	// 現在表示されているのが SaveLoadMenu である場合のみ、メインメニューへ戻る
	if (MenuSwitcher->GetActiveWidget() == WBP_SaveLoadMenu)
	{
		MenuSwitcher->SetActiveWidget(WBP_LinearTitleMenu);

		if (WBP_LinearTitleMenu)
		{
			// 戻った際、コントローラー操作が途切れないようにフォーカスを当て直す
			WBP_LinearTitleMenu->FocusFirstButton();
		}
	}
}
