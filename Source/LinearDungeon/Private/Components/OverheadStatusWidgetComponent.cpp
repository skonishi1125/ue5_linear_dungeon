#include "Components/OverheadStatusWidgetComponent.h"
#include "Components/HUD/EnemyStatusWidget.h"

UEnemyStatusWidget* UOverheadStatusWidgetComponent::GetStatusWidget()
{
	if (!CachedStatusWidget)
	{
		CachedStatusWidget = Cast<UEnemyStatusWidget>(GetUserWidgetObject());
	}
	return CachedStatusWidget;
}

void UOverheadStatusWidgetComponent::SetHealthPercent(float Percent)
{
	if (UEnemyStatusWidget* StatusWidget = GetStatusWidget())
	{
		StatusWidget->SetHealthPercent(Percent);
	}
}
void UOverheadStatusWidgetComponent::SetPoisePercent(float Percent)
{
	if (UEnemyStatusWidget* StatusWidget = GetStatusWidget())
	{
		StatusWidget->SetPoisePercent(Percent);
	}
}

