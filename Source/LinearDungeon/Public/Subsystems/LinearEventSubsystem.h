#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LinearEventSubsystem.generated.h"

// 発生するイベントについて管理する Subsystem
UCLASS()
class LINEARDUNGEON_API ULinearEventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// イベントがクリア済みかどうか
	UFUNCTION(BlueprintCallable, Category = "Events")
	bool IsEventCleared(FName EventID) const;

	// イベントをクリア済みにする処理
	UFUNCTION(BlueprintCallable, Category = "Events")
	void MarkEventAsCleared(FName EventID);

	// 初期化用 New Game時などにフラグをリセットする用途
	UFUNCTION(BlueprintCallable, Category = "Events")
	void ResetAllEvents();

	// --- セーブ/ロード連携用 ---
	const TSet<FName>& GetClearedEvents() const { return ClearedEvents; }
	void SetClearedEvents(const TSet<FName>& InEvents) { ClearedEvents = InEvents; }

private:
	// クリア済みイベントID群
	UPROPERTY()
	TSet<FName> ClearedEvents;

	
};
