#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuContainerWidget.generated.h"

class UWidgetSwitcher;
class ULinearMainMenuUserWidget;
class USaveLoadMenuWidget;


UCLASS()
class LINEARDUNGEON_API UMenuContainerWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// メニューを閉じた後再度開くとき、再び MainMenu に切り替えておく関数
	void ResetToMainMenu();

	// PlayerController などに、WBP_SaveLoadMenu のデリゲートを読ませるためのもの
	FORCEINLINE USaveLoadMenuWidget* GetWBPSaveLoadMenu() { return WBP_SaveLoadMenu; }

protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> MenuSwitcher;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<ULinearMainMenuUserWidget> WBP_LinearMainMenu;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USaveLoadMenuWidget> WBP_SaveLoadMenu;

	// ===== MainMenu からの Delegate を受け取り処理する関数 =====
	UFUNCTION()
	void HandleSaveMenuRequested();

	UFUNCTION()
	void HandleLoadMenuRequested();

};
