#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveLoadMenuWidget.generated.h"

class UTextBlock;
class UButton;

// ロードが押されたことを親に通知するためのDelegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLoadButtonPressedDelegate);


UENUM(BlueprintType)
enum class ESaveLoadMode : uint8
{
	ESL_Save UMETA(DisplayName = "Save Mode"),
	ESL_Load UMETA(DisplayName = "Load Mode")
};

UCLASS()
class LINEARDUNGEON_API USaveLoadMenuWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 画面切り替え時、親から呼ばれる関数
	void ActivateMenu(ESaveLoadMode InMode);

	// ロードしたときに Broadcast する
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLoadButtonPressedDelegate OnLoadButtonPressedDelegate;


protected:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

private:
	// 現在のモードを保持する変数
	ESaveLoadMode CurrentMode = ESaveLoadMode::ESL_Save;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InfoText;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SlotButton_0;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SlotButton_1;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock>SlotTimeStamp_0;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock>SlotTimeStamp_1;

	 UPROPERTY()
	 TArray<TObjectPtr<UButton>> SlotButtons;
	 UPROPERTY()
	 TArray<TObjectPtr<UTextBlock>> SlotTimeStamps;

	 // 全てのスロットの表示を最新にする関数
	 void RefreshSlotDisplay();


	// ボタンクリックイベントのラッパー
	UFUNCTION() void OnSlot0Clicked() { ExecuteSaveOrLoad(0); }
	UFUNCTION() void OnSlot1Clicked() { ExecuteSaveOrLoad(1); }

	// 実際の処理を行うコア関数
	void ExecuteSaveOrLoad(int32 SlotIndex);
	
};
