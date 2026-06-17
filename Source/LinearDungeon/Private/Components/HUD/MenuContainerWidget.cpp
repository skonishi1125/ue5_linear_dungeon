#include "Components/HUD/MenuContainerWidget.h"
#include "Logging/StructuredLog.h"

#include "Components/WidgetSwitcher.h"
#include "Components/HUD/LinearMainMenuUserWidget.h"
#include "Components/HUD/SaveLoadMenuWidget.h"

void UMenuContainerWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (WBP_LinearMainMenu)
	{
		WBP_LinearMainMenu->OnSaveMenuRequestedDelegate.AddDynamic(this, &UMenuContainerWidget::HandleSaveMenuRequested);
		WBP_LinearMainMenu->OnLoadMenuRequestedDelegate.AddDynamic(this, &UMenuContainerWidget::HandleLoadMenuRequested);
	}

}

void UMenuContainerWidget::ResetToMainMenu()
{
	if (MenuSwitcher && WBP_LinearMainMenu)
	{
		// WidgetSwitcherのインデックスをMainMenu（通常は0番目）に切り替える
		MenuSwitcher->SetActiveWidget(WBP_LinearMainMenu);
		// メインメニューの初期フォーカスを再設定
		WBP_LinearMainMenu->SetKeyboardFocus();
	}
}

void UMenuContainerWidget::HandleSaveMenuRequested()
{
	if (MenuSwitcher && WBP_SaveLoadMenu)
	{
		// WidgetSwitcherの表示をSaveLoadMenuに切り替える
		MenuSwitcher->SetActiveWidget(WBP_SaveLoadMenu);

		// Saveモードとしてアクティベート
		WBP_SaveLoadMenu->ActivateMenu(ESaveLoadMode::ESL_Save);
	}
}

void UMenuContainerWidget::HandleLoadMenuRequested()
{
	if (MenuSwitcher && WBP_SaveLoadMenu)
	{
		// WidgetSwitcherの表示をSaveLoadMenuに切り替える
		MenuSwitcher->SetActiveWidget(WBP_SaveLoadMenu);

		// Loadモードとしてアクティベート
		WBP_SaveLoadMenu->ActivateMenu(ESaveLoadMode::ESL_Load);
	}
}
