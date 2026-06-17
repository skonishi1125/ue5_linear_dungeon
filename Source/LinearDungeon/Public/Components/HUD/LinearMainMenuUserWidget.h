#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LinearMainMenuUserWidget.generated.h"

class UButton;

// サブメニュー群にボタンが押されたことを通知するデリゲート
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveMenuRequestedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadMenuRequestedDelegate);

UCLASS()
class LINEARDUNGEON_API ULinearMainMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 親が購読する、各ボタン押下時のデリゲート
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSaveMenuRequestedDelegate OnSaveMenuRequestedDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLoadMenuRequestedDelegate OnLoadMenuRequestedDelegate;


protected:
	// ボタンの OnClick と、押された時の関数を紐づける用途の初期化処理
	virtual bool Initialize() override;

	// Widget が Viewport に追加された時に呼ばれる関数
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SaveButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> LoadButton;

	UFUNCTION()
	void OnSaveButtonClicked();

	UFUNCTION()
	void OnLoadButtonClicked();
	
};
