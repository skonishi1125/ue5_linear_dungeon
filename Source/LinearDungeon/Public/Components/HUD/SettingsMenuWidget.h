#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenuWidget.generated.h"

class UComboBoxString;

UCLASS()
class LINEARDUNGEON_API USettingsMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UComboBoxString> ComboBox_GraphicsQuality;

	// ESelectInfo を使うためには、SlateCore モジュールを Build.cs に入れる必要あり
	UFUNCTION()
	void OnGraphicsQualityChanged(FString SelectedItem, ESelectInfo::Type SelectionType);
	
};
