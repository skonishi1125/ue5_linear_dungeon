#include "Subsystems/LinearSettingsSubsystem.h"
#include "Logging/StructuredLog.h"

#include "GameFramework/GameUserSettings.h"

void ULinearSettingsSubsystem::SetGraphicsQuality(int32 QualityLevel)
{
	if (GEngine)
	{
		// グラフィック設定を管理するために UserSettings を取得
		// ※ UserSettings は所謂、シングルトンのオブジェクト
		// (ゲーム中 1 つしか存在しない。仮に複数あったら、場面によって画質などが変わってしまう)
		UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
		if (UserSettings)
		{
			UserSettings->SetOverallScalabilityLevel(QualityLevel);

			// t/f: コマンドライン側の上書きをチェックするかどうか
			// ゲーム起動時などにコマンドライン側で画質設定を行われていたら、trueの場合そちらを優先する
			// false とすると、今回保持されている画質レベルなどを強制適用する
			UserSettings->ApplySettings(false);
		}
	}
}

int32 ULinearSettingsSubsystem::GetGraphicsQuality() const
{
	if (GEngine)
	{
		UGameUserSettings* UserSettings = GEngine->GetGameUserSettings();
		if (UserSettings)
		{
			// 現在の全体スケーラビリティレベルを返す
			// ※個別にShadow=1, Texture=3などと設定されている場合は -1 が返る仕様に注意
			return UserSettings->GetOverallScalabilityLevel();
		}
	}

	// 取得失敗時は、デフォルトとして Medium を返す
	return 1;

}

void ULinearSettingsSubsystem::SetMouseSensitivity(float ClampedInSensitivity)
{
	MouseSensitivity = ClampedInSensitivity;
	UE_LOGFMT(LogTemp, Warning, "ULinearSettingsSubsystem::SetMouseSensitivity(), {0}", ClampedInSensitivity);
}

void ULinearSettingsSubsystem::SetWindowMode(int32 ModeIndex)
{
	if (GEngine)
	{
		if (UGameUserSettings* UserSettings = GEngine->GetGameUserSettings())
		{
			// 0: FullSC 1: Windowed FullSC 2: Windowed
			EWindowMode::Type TargetMode = EWindowMode::Fullscreen;

			if (ModeIndex == 1) TargetMode = EWindowMode::WindowedFullscreen;
			else if (ModeIndex == 2) TargetMode = EWindowMode::Windowed;

			UserSettings->SetFullscreenMode(TargetMode);
			UserSettings->ApplySettings(false);
		}
	}
}

int32 ULinearSettingsSubsystem::GetWindowMode() const
{
	if (GEngine)
	{
		if (UGameUserSettings* UserSettings = GEngine->GetGameUserSettings())
		{
			EWindowMode::Type CurrentMode = UserSettings->GetFullscreenMode();

			if (CurrentMode == EWindowMode::WindowedFullscreen) return 1;
			if (CurrentMode == EWindowMode::Windowed) return 2;

			return 0; // FullScreen の場合は、0 を返す
		}
	}
	return 0;
}
