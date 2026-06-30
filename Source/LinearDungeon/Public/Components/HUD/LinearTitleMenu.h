#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "LinearTitleMenu.generated.h"

class UButton;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTitleNewGameRequestedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTitleLoadMenuRequestedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTitleSettingsRequestedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTitleQuitGameRequestedDelegate);

UCLASS()
class LINEARDUNGEON_API ULinearTitleMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	void FocusFirstButton();

	// === TitleMenu の 配置した各ボタン が押されたことを通知する ===
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTitleNewGameRequestedDelegate OnTitleNewGameRequestedDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTitleLoadMenuRequestedDelegate OnTitleLoadMenuRequestedDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTitleSettingsRequestedDelegate OnTitleSettingsRequestedDelegate;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnTitleQuitGameRequestedDelegate OnTitleQuitGameRequestedDelegate;

protected:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NewGameButton;
	UFUNCTION()
	void OnNewGameButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LoadButton;
	UFUNCTION()
	void OnLoadButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SettingsButton;
	UFUNCTION()
	void OnSettingsButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> QuitGameButton;
	UFUNCTION()
	void OnQuitGameButtonClicked();

	
};
