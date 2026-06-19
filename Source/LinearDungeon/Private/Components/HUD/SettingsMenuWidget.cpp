#include "Components/HUD/SettingsMenuWidget.h"
#include "Logging/StructuredLog.h"

#include "Components/ComboBoxString.h"
#include "Subsystems/LinearSettingsSubsystem.h"
#include "Components/Slider.h"
#include "Subsystems/LinearAudioSubsystem.h"

void USettingsMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ComboBox_GraphicsQuality)
	{
		ComboBox_GraphicsQuality->ClearOptions();
		ComboBox_GraphicsQuality->AddOption(TEXT("Low"));
		ComboBox_GraphicsQuality->AddOption(TEXT("Medium"));
		ComboBox_GraphicsQuality->AddOption(TEXT("High"));
		ComboBox_GraphicsQuality->AddOption(TEXT("Epic"));

		// Subsystem から現在の設定を取得して、UI に反映
		if (UGameInstance* GI = GetGameInstance())
		{
			if (ULinearSettingsSubsystem* SettingsSubsystem = GI->GetSubsystem<ULinearSettingsSubsystem>())
			{
				int32 CurrentQuality = SettingsSubsystem->GetGraphicsQuality();
				CurrentQuality = FMath::Clamp(CurrentQuality, 0, 3); // 予期しない数値が入った時は、範囲内に抑える
				ComboBox_GraphicsQuality->SetSelectedIndex(CurrentQuality);
			}
		}

		// UI で設定を変更したときに発火する関数を紐づける
		ComboBox_GraphicsQuality->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget::OnGraphicsQualityChanged);
	}

	if (BGMSlider)
	{
		BGMSlider->OnValueChanged.AddDynamic(this, &USettingsMenuWidget::OnBGMSliderValueChanged);
	}

}

// SettingsMenu ComboBox で設定変更時に発火する関数
void USettingsMenuWidget::OnGraphicsQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	// とりあえず、仮値で Default = Medium 設定
	int32 QualityLevel = 1;
	if (SelectedItem == TEXT("Low")) QualityLevel = 0;
	else if (SelectedItem == TEXT("Medium")) QualityLevel = 1;
	else if (SelectedItem == TEXT("High")) QualityLevel = 2;
	else if (SelectedItem == TEXT("Epic")) QualityLevel = 3;

	// 設定用 ULinearSettingsSubsystem を呼び出して GameUserSettings に適用
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULinearSettingsSubsystem* SettingsSubsystem = GI->GetSubsystem<ULinearSettingsSubsystem>())
		{
			SettingsSubsystem->SetGraphicsQuality(QualityLevel);
		}
	}
}

void USettingsMenuWidget::OnBGMSliderValueChanged(float Value)
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULinearAudioSubsystem* AudioSubsystem = GI->GetSubsystem<ULinearAudioSubsystem>())
		{
			float ClampedVolume = FMath::Clamp(Value, 0.01f, 1.0f);
			AudioSubsystem->SetBGMVolume(ClampedVolume);
			UE_LOGFMT(LogTemp, Warning, "USettingsMenuWidget::OnBGMSliderValueChanged() {0}", ClampedVolume);
		}
	}
}
