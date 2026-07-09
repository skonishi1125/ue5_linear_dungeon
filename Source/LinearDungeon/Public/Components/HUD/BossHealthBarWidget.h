#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"

#include "BossHealthBarWidget.generated.h"

class UAttributeComponent;
class UProgressBar;
class UTextBlock;

UCLASS()
class LINEARDUNGEON_API UBossHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealthPercent(float Percent);
	void SetPoisePercent(float Percent);
	void SetBossName(const FText& InName);
	void BindToAttributes(UAttributeComponent* InAttributes);
	void UnbindFromAttributes();

protected:
	virtual void NativeDestruct() override;

private:
	UPROPERTY()
	TObjectPtr<UAttributeComponent> CachedAttributes;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PoiseProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> BossNameText;

	UFUNCTION()
	void OnHealthPercentChanged(float NewPercent);
	
	UFUNCTION()
	void OnPoisePercentChanged(float NewPercent);

};
