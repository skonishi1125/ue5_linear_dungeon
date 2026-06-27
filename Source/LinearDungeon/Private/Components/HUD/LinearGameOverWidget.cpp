#include "Components/HUD/LinearGameOverWidget.h"
#include "Animation/WidgetAnimation.h"

void ULinearGameOverWidget::PlayFadeInAnimation()
{
	if (FadeInAnim)
	{
		PlayAnimation(FadeInAnim);
	}
}

void ULinearGameOverWidget::PlayTextFadeOutAnimation()
{
	if (TextFadeOutAnim)
	{
		PlayAnimation(TextFadeOutAnim);
	}
}