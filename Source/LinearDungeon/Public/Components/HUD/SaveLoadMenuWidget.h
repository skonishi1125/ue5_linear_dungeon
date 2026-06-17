#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SaveLoadMenuWidget.generated.h"

class UTextBlock;
class UButton;

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

protected:
	virtual bool Initialize() override;

private:
	// 現在のモードを保持する変数
	ESaveLoadMode CurrentMode = ESaveLoadMode::ESL_Save;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InfoText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SlotButton_0;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> SlotButton_1;

	// ボタンクリックイベントのラッパー
	UFUNCTION() void OnSlot0Clicked() { ExecuteSaveOrLoad(0); }
	UFUNCTION() void OnSlot1Clicked() { ExecuteSaveOrLoad(1); }

	// 実際の処理を行うコア関数
	void ExecuteSaveOrLoad(int32 SlotIndex);
	
};
