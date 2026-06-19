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

		// Subsystem ‚©‚çŒ»Ý‚ÌÝ’è‚ðŽæ“¾‚µ‚ÄAUI ‚É”½‰f
		if (UGameInstance* GI = GetGameInstance())
		{
			if (ULinearSettingsSubsystem* SettingsSubsystem = GI->GetSubsystem<ULinearSettingsSubsystem>())
			{
				int32 CurrentQuality = SettingsSubsystem->GetGraphicsQuality();
				CurrentQuality = FMath::Clamp(CurrentQuality, 0, 3); // —\Šú‚µ‚È‚¢”’l‚ª“ü‚Á‚½Žž‚ÍA”ÍˆÍ“à‚É—}‚¦‚é
				ComboBox_GraphicsQuality->SetSelectedIndex(CurrentQuality);
			}
		}

		// UI ‚ÅÝ’è‚ð•ÏX‚µ‚½‚Æ‚«‚É”­‰Î‚·‚éŠÖ”‚ð•R‚Ã‚¯‚é
		ComboBox_GraphicsQuality->OnSelectionChanged.AddDynamic(this, &USettingsMenuWidget::OnGraphicsQualityChanged);
	}

	if (BGMSlider)
	{
		// w“Ç
		BGMSlider->OnValueChanged.AddDynamic(this, &USettingsMenuWidget::OnBGMSliderValueChanged);

		// Slider ‚É‰Šú’l”½‰f
		if (UGameInstance* GI = GetGameInstance())
		{
			if (ULinearAudioSubsystem* AudioSubsystem = GI->GetSubsystem<ULinearAudioSubsystem>())
			{
				BGMSlider->SetValue(AudioSubsystem->GetCurrentBGMVolume());
			}
		}
	}

}

// SettingsMenu ComboBox ‚ÅÝ’è•ÏXŽž‚É”­‰Î‚·‚éŠÖ”
void USettingsMenuWidget::OnGraphicsQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType)
{
	// ‚Æ‚è‚ ‚¦‚¸A‰¼’l‚Å Default = Medium Ý’è
	int32 QualityLevel = 1;
	if (SelectedItem == TEXT("Low")) QualityLevel = 0;
	else if (SelectedItem == TEXT("Medium")) QualityLevel = 1;
	else if (SelectedItem == TEXT("High")) QualityLevel = 2;
	else if (SelectedItem == TEXT("Epic")) QualityLevel = 3;

	// Ý’è—p ULinearSettingsSubsystem ‚ðŒÄ‚Ño‚µ‚Ä GameUserSettings ‚É“K—p
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
