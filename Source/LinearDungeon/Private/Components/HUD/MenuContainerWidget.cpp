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
		
		MenuSwitcher->SetActiveWidget(WBP_LinearMainMenu); // WidgetSwitcher の Index を 0(MainMenu) に切り替えておく
		WBP_LinearMainMenu->FocusFirstButton(); // メインメニューの初期キーフォーカス先ボタンを再度設定
	}
}

void UMenuContainerWidget::HandleSaveMenuRequested()
{
	if (MenuSwitcher && WBP_SaveLoadMenu)
	{
		// WidgetSwitcherの表示をSaveLoadMenuに切り替え、セーブモードとして軌道
		MenuSwitcher->SetActiveWidget(WBP_SaveLoadMenu);
		WBP_SaveLoadMenu->ActivateMenu(ESaveLoadMode::ESL_Save);
	}
}

void UMenuContainerWidget::HandleLoadMenuRequested()
{
	if (MenuSwitcher && WBP_SaveLoadMenu)
	{
		MenuSwitcher->SetActiveWidget(WBP_SaveLoadMenu);
		WBP_SaveLoadMenu->ActivateMenu(ESaveLoadMode::ESL_Load);
	}
}
