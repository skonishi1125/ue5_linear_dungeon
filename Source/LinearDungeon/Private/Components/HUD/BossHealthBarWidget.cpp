#include "Components/HUD/BossHealthBarWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/AttributeComponent.h"

void UBossHealthBarWidget::NativeDestruct()
{
	UnbindFromAttributes();
	Super::NativeDestruct();
}

void UBossHealthBarWidget::SetHealthPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void UBossHealthBarWidget::SetPoisePercent(float Percent)
{
	if (PoiseProgressBar)
	{
		PoiseProgressBar->SetPercent(Percent);
	}
}

void UBossHealthBarWidget::SetBossName(const FText& InName)
{
	if (BossNameText)
	{
		BossNameText->SetText(InName);
	}
}

void UBossHealthBarWidget::BindToAttributes(UAttributeComponent* InAttributes)
{
	if (!InAttributes)
	{
		return;
	}

	UnbindFromAttributes(); // “ñdƒoƒCƒ“ƒh–hŽ~

	CachedAttributes = InAttributes;
	CachedAttributes->OnHealthPercentChanged.AddUniqueDynamic(
		this, &UBossHealthBarWidget::OnHealthPercentChanged
	);
	CachedAttributes->OnPoisePercentChanged.AddUniqueDynamic(
		this, &UBossHealthBarWidget::OnPoisePercentChanged
	);

	// ‰Šú’l‚ð1‰ñ”½‰f
	OnHealthPercentChanged(CachedAttributes->GetHealthPercent());
	OnPoisePercentChanged(CachedAttributes->GetPoisePercent());
}

void UBossHealthBarWidget::UnbindFromAttributes()
{
	if (!CachedAttributes)
	{
		return;
	}
	CachedAttributes->OnHealthPercentChanged.RemoveDynamic(
		this, &UBossHealthBarWidget::OnHealthPercentChanged
	);
	CachedAttributes->OnPoisePercentChanged.RemoveDynamic(
		this, &UBossHealthBarWidget::OnPoisePercentChanged
	);
	CachedAttributes = nullptr;
}

void UBossHealthBarWidget::OnHealthPercentChanged(float NewPercent)
{
	SetHealthPercent(NewPercent);
}

void UBossHealthBarWidget::OnPoisePercentChanged(float NewPercent)
{
	SetPoisePercent(NewPercent);
}