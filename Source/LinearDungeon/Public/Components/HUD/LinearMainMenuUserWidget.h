#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LinearMainMenuUserWidget.generated.h"

class UButton;

// サブメニュー群にボタンが押されたことを通知するデリゲート
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveMenuRequestedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadMenuRequestedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSettingsMenuRequestedDelegate);

UCLASS()
class LINEARDUNGEON_API ULinearMainMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void FocusFirstButton();

	// 親が購読する、各ボタン押下時のデリゲート
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSaveMenuRequestedDelegate OnSaveMenuRequestedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLoadMenuRequestedDelegate OnLoadMenuRequestedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSettingsMenuRequestedDelegate OnSettingsMenuRequestedDelegate;

protected:
	// ボタンの OnClick と、押された時の関数を紐づける用途の初期化処理
	virtual bool Initialize() override;

	// Widget が Viewport に追加された時に呼ばれる関数
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SaveButton;
	UFUNCTION()
	void OnSaveButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LoadButton;
	UFUNCTION()
	void OnLoadButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SettingsButton;
	UFUNCTION()
	void OnSettingsButtonClicked();

	
};
