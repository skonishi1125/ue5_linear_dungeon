#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LinearTitleMenuContainer.generated.h"

class UWidgetSwitcher;
class ULinearTitleMenu;
class USaveLoadMenuWidget;
class USettingsMenuWidget;

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
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USettingsMenuWidget> WBP_SettingsMenu;
	
	// LinearTitleMenu ‚Å LoadButton ‚ğ‰Ÿ‚µ‚½‚Æ‚«‚Ìˆ—
	UFUNCTION()
	void HandleTitleNewGameRequested();
	UFUNCTION()
	void HandleTitleLoadMenuRequested();
	UFUNCTION()
	void HandleTitleSettingsMenuRequested();
	UFUNCTION()
	void HandleTitleQuitGameRequested();

	// TitlePlayerController ‚Å CloseTitleSubMenu ‚ª‰Ÿ‚³‚ê‚½
	UFUNCTION()
	void HandleCancelInput();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TSoftObjectPtr<UWorld> NextLevel;
};
