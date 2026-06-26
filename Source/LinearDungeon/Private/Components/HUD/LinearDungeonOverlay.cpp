#include "Components/HUD/LinearDungeonOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

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

void ULinearDungeonOverlay::SetNumberOfPotionText(int32 NumberOfPotion)
{
	if (PotionText)
	{
		// FText::Format を使用して "× {0}" の {0} 部分に数値を代入する
		// FText::AsNumber() は、ロケールに応じた数値フォーマット（例: 1,000）を自動で適用する
		// ※ファイル自体を UTF-8 with signature で 保存する
		FText FormattedText = FText::Format(
			FText::FromString(TEXT("× {0}")),
			FText::AsNumber(NumberOfPotion)
		);

		PotionText->SetText(FormattedText);
	}
}
