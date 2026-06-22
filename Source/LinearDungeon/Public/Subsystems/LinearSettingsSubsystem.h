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
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Settings 保存用
	UFUNCTION()
	void SaveSettings();

	// BGMVolume の設定と取得
	// ※実際に音量を変更 / 再生 /停止する処理は AudioSubsystem で行われる
	// ただし、UI -> Settings -> Audio と中継するようにして、SettingsSaveGame に保管できるようにしている
	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	void SetBGMVolume(float ClampedVolume);
	UFUNCTION(BlueprintCallable, Category = "Settings|Audio")
	FORCEINLINE float GetBGMVolume() { return ThisBGMVolume; }

	// 画質設定適用 0: Low, 1: Medium, 2: High, 3: Epic
	UFUNCTION(BlueprintCallable, Category = "Settings|Display")
	void SetGraphicsQuality(int32 QualityLevel);
	UFUNCTION(BlueprintCallable, Category = "Settings|Display")
	int32 GetGraphicsQuality() const; // 現在の画質設定取得用（UI閲覧時の初期化用）

	// マウス感度の設定と取得
	UFUNCTION(BlueprintCallable, Category = "Settings|Input")
	void SetMouseSensitivity(float ClampedInSensitivity);
	UFUNCTION(BlueprintPure, Category = "Settings|Input")
	FORCEINLINE float GetMouseSensitivity() const { return ThisMouseSensitivity; }

	// Window Mode 設定
	UFUNCTION(BlueprintCallable, Category = "Settings|Display")
	void SetWindowMode(int32 ModeIndex);
	UFUNCTION(BlueprintCallable, Category = "Settings|Display")
	int32 GetWindowMode() const;

	// 解像度
	UFUNCTION(BlueprintCallable, Category = "Settings|Display")
	void SetResolutionByIndex(int32 Index);
	UFUNCTION(BlueprintCallable, Category = "Settings|Display")
	int32 GetResolutionIndex() const;

private:
	// Settings だけを保管するスロットの名前
	const FString SettingsSlotName = TEXT("GlobalSettings");

	UPROPERTY()
	float ThisMouseSensitivity = 1.0f; // マウス感度のデフォルト値

	UPROPERTY()
	float ThisBGMVolume = 1.0f; // BGM Volumeデフォルト値
	
};
