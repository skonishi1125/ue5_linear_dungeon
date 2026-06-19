#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LinearSettingsSubsystem.generated.h"

// 画質等の調整用 Subsystem
UCLASS()
class LINEARDUNGEON_API ULinearSettingsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	// 画質設定適用 0: Low, 1: Medium, 2: High, 3: Epic
	UFUNCTION(BlueprintCallable, Category = "Settings")
	void SetGraphicsQuality(int32 QualityLevel);

	// 現在の画質設定取得用（UI閲覧時の初期化用）
	UFUNCTION(BlueprintPure, Category = "Settings")
	int32 GetGraphicsQuality() const;

	
};
