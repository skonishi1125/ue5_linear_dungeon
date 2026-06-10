#include "Components/HUD/LinearDungeonOverlay.h"
#include "Components/ProgressBar.h"

void ULinearDungeonOverlay::SetHealthBarPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void ULinearDungeonOverlay::SetPoiseBarPercent(float Percent)
{
	if (PoiseProgressBar)
	{
		PoiseProgressBar->SetPercent(Percent);
	}
}
