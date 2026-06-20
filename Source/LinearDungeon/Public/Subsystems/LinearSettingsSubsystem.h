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
	UFUNCTION(BlueprintPure, Category = "Settings")
	int32 GetGraphicsQuality() const; // 現在の画質設定取得用（UI閲覧時の初期化用）

	// マウス感度の設定と取得
	UFUNCTION(BlueprintCallable, Category = "Settings|Input")
	void SetMouseSensitivity(float ClampedInSensitivity);
	UFUNCTION(BlueprintPure, Category = "Settings|Input")
	FORCEINLINE float GetMouseSensitivity() const { return MouseSensitivity; }

	// Window Mode 設定
	UFUNCTION(BlueprintCallable, Category = "Settings|Display")
	void SetWindowMode(int32 ModeIndex);
	UFUNCTION(BlueprintCallable, Category = "Settings|Display")
	int32 GetWindowMode() const;


private:
	// マウス感度 デフォルト値 1.0f
	UPROPERTY()
	float MouseSensitivity = 1.0f;
	
};
