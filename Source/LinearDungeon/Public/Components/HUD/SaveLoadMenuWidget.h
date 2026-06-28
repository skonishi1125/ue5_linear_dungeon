#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveLoadMenuWidget.generated.h"

class UTextBlock;
class UButton;
class UCircularThrobber;

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
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	// 現在のモードを保持する変数
	ESaveLoadMode CurrentMode = ESaveLoadMode::ESL_Save;

	// ===== Widget 内に設置する要素 =====
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InfoText;

	// AutoSaveSlot
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SlotButton_0;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock>SlotTimeStamp_0;
	
	// 通常 Slot
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SlotButton_1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock>SlotTimeStamp_1;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SlotButton_2;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock>SlotTimeStamp_2;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SlotButton_3;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock>SlotTimeStamp_3;

	// ぐるぐる回るロード画面
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidget> Overlay_Processing;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ProcessingInfo;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCircularThrobber> Circular_ProcessingInfo;

	// ボタン, タイムスタンプの配列
	UPROPERTY()
	TArray<TObjectPtr<UButton>> SlotButtons;
	UPROPERTY()
	TArray<TObjectPtr<UTextBlock>> SlotTimeStamps;
	
	// ボタンクリックイベントのラッパーと、実際行うコア関数
	UFUNCTION() void OnSlot0Clicked() { ExecuteSaveOrLoad(0); }
	UFUNCTION() void OnSlot1Clicked() { ExecuteSaveOrLoad(1); }
	UFUNCTION() void OnSlot2Clicked() { ExecuteSaveOrLoad(2); }
	UFUNCTION() void OnSlot3Clicked() { ExecuteSaveOrLoad(3); }
	void ExecuteSaveOrLoad(int32 SlotIndex);

	 // 全てのスロットの表示を最新にする関数
	 void RefreshSlotDisplay();

	// SaveSubsystem の  セーブ / ロード完了時の処理 Delegate と紐づける
	UFUNCTION()
	void OnSaveCompleted();
	
	// セーブロード実行時のUIを操作するための変数, 関数など
	bool bIsProcessingWait = false;
	float ProcessingWaitTime = 0.0f;
	void FinishProcessingUI();

	// 選択されたスロットを記憶しておく変数
	int32 SelectedSlotIndex = -1;

};
