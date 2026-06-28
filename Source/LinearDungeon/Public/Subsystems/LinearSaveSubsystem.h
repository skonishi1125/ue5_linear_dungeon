#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LinearSaveSubsystem.generated.h"

class ULinearSaveGame;
class ALinearPlayerCharacter;
class USaveGame;

// Save / Load 完了通知用デリゲート UI更新などに使う
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveCompletedDelegate);

UCLASS()
class LINEARDUNGEON_API ULinearSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "SaveSystem")
	FOnSaveCompletedDelegate OnSaveCompleted;

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SaveGame(int32 SlotIndex);
	void SaveGameCompleted(const FString& SlotName, const int32 UserIndex, bool bSuccess);

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void LoadGame(int32 SlotIndex);
	void LoadGameCompleted(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);

	// ゲーム開始 | Die()後 リトライ処理関連
	// 次のレベルロード時に読み込むセーブデータを予約する
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SetPendingLoad(int32 SlotIndex);
	// 予約されたロードがあるか確認し、あればそちらを読み込む
	bool ConsumePendingLoad(int32& OutSlotIndex);

	
private:
	// 固定のスロット名
	const FString BaseSaveSlotName = TEXT("SaveSlot_");

	bool bHasPendingLoad = false;
	int32 PendingLoadSlotIndex = -1;
};
