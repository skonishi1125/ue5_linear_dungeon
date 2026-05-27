#include "Components/HUD/HealthBarComponent.h"
#include "Components/HUD/HealthBar.h"
#include "Components/ProgressBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{
	// ↓は体力の変動があるたびキャストして格納する形
	// コストのある処理なので、メンバ変数を作って一度だけ格納する形にする
	// UHealthBar* HealthBar = Cast<UHealthBar>(GetUserWidgetObject());
	
	if (HealthBarWidget == nullptr)
	{
		// GetUserWidgetObject()
		// User Widget のインスタンスのポインタを取得する変数
		// 今回、UHealthBar の基底である UUserWidget* が返る。そのため、ダウンキャストで対応
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	}

	if (HealthBarWidget && HealthBarWidget->HealthBar)
	{
		// ProgressBar のメソッドを使って % 指定
		HealthBarWidget->HealthBar->SetPercent(Percent);
	}

}
