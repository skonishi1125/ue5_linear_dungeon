#include "Components/HUD/SettingsMenuWidget.h"
#include "Logging/StructuredLog.h"

#include "Components/ComboBoxString.h"
#include "Subsystems/LinearSettingsSubsystem.h"
#include "Components/Slider.h"
#include "Subsystems/LinearAudioSubsystem.h"

void USettingsMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	UGameInstance* GI = GetGameInstance();

	if (ComboBox_GraphicsQuality && GI)
	{
		ComboBox_GraphicsQuality->ClearOptions();
		ComboBox_GraphicsQuality->AddOption(TEXT("Low"));
		ComboBox_GraphicsQuality->AddOption(TEXT("Medium"));
		ComboBox_GraphicsQuality->AddOption(TEXT("High"));
		ComboBox_GraphicsQuality->AddOption(TEXT("Epic"));

		// Subsystem から現在の設定を取得して、UI に反映
		if (ULinearSettingsSubsystem* SettingsSubsystem = GI->GetSubsystem<ULinearSettingsSubsystem>())
		{
			int32 CurrentQuality = SettingsSubsystem->GetGraphicsQuality();
			CurrentQuality = FMath::Clamp(CurrentQuality, 0, 3); // 予期しない数値が入った時は、範囲内に抑える
			ComboBox_GraphicsQuality->SetSelectedIndex(CurrentQuality);
		}

		// UI で設定を変更したときに発火する関数を紐づける
		ComboBox_GraphicsQuality->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget::OnGraphicsQualityChanged);
	}

	if (BGMSlider && GI)
	{
		// 購読
		BGMSlider->OnValueChanged.AddDynamic(this, &USettingsMenuWidget::OnBGMSliderValueChanged);

		// Slider に初期値反映
		if (ULinearAudioSubsystem* AudioSubsystem = GI->GetSubsystem<ULinearAudioSubsystem>())
		{
			BGMSlider->SetValue(AudioSubsystem->GetCurrentBGMVolume());
		}
	}

	if (MouseSensitivitySlider && GI)
	{
		// 購読、初期値反映
		MouseSensitivitySlider->OnValueChanged.AddDynamic(this, &USettingsMenuWidget::OnMouseSensitivitySliderValueChanged);
		if (ULinearSettingsSubsystem* SettingsSubSystem = GI->GetSubsystem<ULinearSettingsSubsystem>())
		{
			MouseSensitivitySlider->SetValue(SettingsSubSystem->GetMouseSensitivity());
		}

	}

	if (ComboBox_WindowMode && GI)
	{
		ComboBox_WindowMode->ClearOptions();
		ComboBox_WindowMode->AddOption(TEXT("Fullscreen"));
		ComboBox_WindowMode->AddOption(TEXT("Windowed Fullscreen"));
		ComboBox_WindowMode->AddOption(TEXT("Windowed"));

		if (ULinearSettingsSubsystem* SettingsSubsystem = GI->GetSubsystem<ULinearSettingsSubsystem>())
		{
			int32 CurrentMode = SettingsSubsystem->GetWindowMode();
			CurrentMode = FMath::Clamp(CurrentMode, 0, 2);
			ComboBox_WindowMode->SetSelectedIndex(CurrentMode);
		}

		ComboBox_WindowMode->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget::OnWindowModeChanged);
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

void USettingsMenuWidget::OnMouseSensitivitySliderValueChanged(float Value)
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULinearSettingsSubsystem* SettingsSubsystem = GI->GetSubsystem<ULinearSettingsSubsystem>())
		{
			float ClampedValue = FMath::Clamp(Value, 0.01f, 2.0f);
			SettingsSubsystem->SetMouseSensitivity(ClampedValue);
		}
	}
}

void USettingsMenuWidget::OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	int32 ModeIndex = 0; // デフォルトでフルスクリーン
	if (SelectedItem == TEXT("Windowed Fullscreen")) ModeIndex = 1;
	else if (SelectedItem == TEXT("Windowed")) ModeIndex = 2;

	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULinearSettingsSubsystem* SettingsSubsystem = GI->GetSubsystem<ULinearSettingsSubsystem>())
		{
			SettingsSubsystem->SetWindowMode(ModeIndex);
		}
	}
}
