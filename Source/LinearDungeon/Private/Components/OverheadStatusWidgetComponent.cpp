#include "Components/OverheadStatusWidgetComponent.h"
#include "Components/HUD/EnemyStatusWidget.h"
#include "Components/AttributeComponent.h"

void UOverheadStatusWidgetComponent::BeginPlay()
{
	Super::BeginPlay();
	BindToAttributes();
}

void UOverheadStatusWidgetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindFromAttributes();
	Super::EndPlay(EndPlayReason);
}

void UOverheadStatusWidgetComponent::BindToAttributes()
{
	// バインドされている Actor から、Attributes を取得して Delegate 登録
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	CachedAttributes = Owner->FindComponentByClass<UAttributeComponent>();
	if (!CachedAttributes)
	{
		return;
	}

	CachedAttributes->OnHealthPercentChanged.AddUniqueDynamic(
		this, &UOverheadStatusWidgetComponent::OnHealthPercentChanged
	);
	CachedAttributes->OnPoisePercentChanged.AddUniqueDynamic(
		this, &UOverheadStatusWidgetComponent::OnPoisePercentChanged
	);

	// 初期値を手動で1回反映（プレイヤーと同じ）
	// 以降、Attributes->ReceiveHealthDamage() を EnemyBase 等で呼ぶ
	// -> この処理の中に、Broadcast() が仕込まれている
	// -> delegate が走る
	// -> このクラス内の OnHealthPercentChanged() が呼ばれる
	// -> SetHealthPercent が呼ばれる
	// -> Widget 更新 という流れになる
	OnHealthPercentChanged(CachedAttributes->GetHealthPercent());
	OnPoisePercentChanged(CachedAttributes->GetPoisePercent());

}

void UOverheadStatusWidgetComponent::OnHealthPercentChanged(float NewPercent)
{
	SetHealthPercent(NewPercent);
}

void UOverheadStatusWidgetComponent::SetHealthPercent(float Percent)
{
	// 初回で呼ぶときなどに、Overhead 側で、Widget 情報を持っていない
	// 2 回目以降の取得を省略するために、こんな感じで Cache して、以降使いまわす設計にする
	if (UEnemyStatusWidget* StatusWidget = GetStatusWidget())
	{
		StatusWidget->SetHealthPercent(Percent);
	}
}

void UOverheadStatusWidgetComponent::OnPoisePercentChanged(float NewPercent)
{
	SetPoisePercent(NewPercent);
}


void UOverheadStatusWidgetComponent::SetPoisePercent(float Percent)
{
	if (UEnemyStatusWidget* StatusWidget = GetStatusWidget())
	{
		StatusWidget->SetPoisePercent(Percent);
	}
}


void UOverheadStatusWidgetComponent::UnbindFromAttributes()
{
	if (!CachedAttributes)
	{
		return;
	}
	CachedAttributes->OnHealthPercentChanged.RemoveDynamic(
		this, &UOverheadStatusWidgetComponent::OnHealthPercentChanged
	);
	CachedAttributes->OnPoisePercentChanged.RemoveDynamic(
		this, &UOverheadStatusWidgetComponent::OnPoisePercentChanged
	);
}


UEnemyStatusWidget* UOverheadStatusWidgetComponent::GetStatusWidget()
{
	if (!CachedStatusWidget)
	{
		CachedStatusWidget = Cast<UEnemyStatusWidget>(GetUserWidgetObject());
	}
	return CachedStatusWidget;
}


