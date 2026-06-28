#include "Components/HUD/LinearRestartWidget.h"
#include "Animation/WidgetAnimation.h"

void ULinearRestartWidget::PlayFadeOutAnimation()
{
	if (FadeOutAnim)
	{
		PlayAnimation(FadeOutAnim);
	}
}
