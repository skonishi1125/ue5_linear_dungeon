#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "OverheadStatusWidgetComponent.generated.h"

class UEnemyStatusWidget;

UCLASS()
class LINEARDUNGEON_API UOverheadStatusWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
public:
	void SetHealthPercent(float Percent);
	void SetPoisePercent(float Percent);
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UEnemyStatusWidget> CachedStatusWidget;
	UEnemyStatusWidget* GetStatusWidget();
};
