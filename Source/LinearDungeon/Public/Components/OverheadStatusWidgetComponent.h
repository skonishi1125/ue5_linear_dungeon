#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "OverheadStatusWidgetComponent.generated.h"

class UEnemyStatusWidget;
class UAttributeComponent;

UCLASS()
class LINEARDUNGEON_API UOverheadStatusWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	void SetHealthPercent(float Percent);
	void SetPoisePercent(float Percent);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY()
	TObjectPtr<UAttributeComponent> CachedAttributes;
	TObjectPtr<UEnemyStatusWidget> CachedStatusWidget;
	
	UEnemyStatusWidget* GetStatusWidget();

	void BindToAttributes();
	void UnbindFromAttributes();
	UFUNCTION()
	void OnHealthPercentChanged(float NewPercent);
	UFUNCTION()
	void OnPoisePercentChanged(float NewPercent);
};
