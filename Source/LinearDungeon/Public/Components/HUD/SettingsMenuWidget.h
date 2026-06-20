#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenuWidget.generated.h"

class UComboBoxString;
class USlider;

UCLASS()
class LINEARDUNGEON_API USettingsMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ComboBox_GraphicsQuality;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> BGMSlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> MouseSensitivitySlider;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ComboBox_WindowMode;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ComboBox_Resolution;

	// ESelectInfo を使うためには、SlateCore モジュールを Build.cs に入れる必要あり
	UFUNCTION()
	void OnGraphicsQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnBGMSliderValueChanged(float Value);
	UFUNCTION()
	void OnMouseSensitivitySliderValueChanged(float Value);
	UFUNCTION()
	void OnWindowModeChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	UFUNCTION()
	void OnResolutionChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	
};
