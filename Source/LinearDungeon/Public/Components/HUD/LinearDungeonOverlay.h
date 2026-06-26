#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LinearDungeonOverlay.generated.h"

class UTextBlock;
class UProgressBar;

UCLASS()
class LINEARDUNGEON_API ULinearDungeonOverlay : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetHealthBarPercent(float Percent);
	void SetPoiseBarPercent(float Percent);
	void SetNumberOfPotionText(int32 NumberOfPotion);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PoiseProgressBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PotionText;
};
