#include "Subsystems/LinearSettingsSubsystem.h"
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
