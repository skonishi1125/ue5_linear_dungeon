#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LinearTitleMenuContainer.generated.h"

class UWidgetSwitcher;
class ULinearTitleMenu;
class USaveLoadMenuWidget;

UCLASS()
class LINEARDUNGEON_API ULinearTitleMenuContainer : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> MenuSwitcher;

	// Switch ‚·‚é WBP ‚ğ‘‚¢‚Ä‚¢‚­
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<ULinearTitleMenu> WBP_LinearTitleMenu;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USaveLoadMenuWidget> WBP_SaveLoadMenu;
	
	// LinearTitleMenu ‚Å LoadButton ‚ğ‰Ÿ‚µ‚½‚Æ‚«‚Ìˆ—
	UFUNCTION()
	void HandleTitleNewGameRequested();
	UFUNCTION()
	void HandleTitleLoadMenuRequested();
	UFUNCTION()
	void HandleTitleQuitGameRequested();

	// TitlePlayerController ‚Å CloseTitleSubMenu ‚ª‰Ÿ‚³‚ê‚½
	UFUNCTION()
	void HandleCancelInput();
};
