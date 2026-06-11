#include "Components/HUD/EnemyStatusWidget.h"
#include "Components/ProgressBar.h"


void UEnemyStatusWidget::SetHealthPercent(float Percent)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(Percent);
	}
}

void UEnemyStatusWidget::SetPoisePercent(float Percent)
{
	if (PoiseBar)
	{
		PoiseBar->SetPercent(Percent);
	}
}