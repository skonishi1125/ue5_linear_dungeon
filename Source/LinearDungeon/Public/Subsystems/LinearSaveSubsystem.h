#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LinearSaveSubsystem.generated.h"

class ULinearSaveGame;
class ALinearPlayerCharacter;
class USaveGame;

// Save / Load 完了通知用デリゲート UI更新などに使う
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveLoadCompletedDelegate);

UCLASS()
class LINEARDUNGEON_API ULinearSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "SaveSystem")
	FOnSaveLoadCompletedDelegate OnSaveLoadCompleted;

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SaveGame(int32 SlotIndex);
	void SaveGameCompleted(const FString& SlotName, const int32 UserIndex, bool bSuccess);

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void LoadGame(int32 SlotIndex);
	void LoadGameCompleted(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData);

	
private:
	// 固定のスロット名
	const FString BaseSaveSlotName = TEXT("SaveSlot_");
};
