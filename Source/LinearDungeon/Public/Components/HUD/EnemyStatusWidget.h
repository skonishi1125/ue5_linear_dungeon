#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyStatusWidget.generated.h"

class UProgressBar;

UCLASS()
class LINEARDUNGEON_API UEnemyStatusWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PoiseBar;
	void SetHealthPercent(float Percent);
	void SetPoisePercent(float Percent);

};
