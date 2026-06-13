#include "Components/HUD/LinearDialogueWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

void ULinearDialogueWidget::StartDialogueText(const FText& InText)
{
	if (!MessageText) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TypewriterTimerHandle);
	}

	FullString = InText.ToString();
	CurrentCharIndex = 0;
	bIsTyping = true;

	MessageText->SetText(FText::GetEmpty());

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			TypewriterTimerHandle,
			this,
			&ULinearDialogueWidget::UpdateTyping,
			TypewriterSpeed,
			true
		);
	}

}


void ULinearDialogueWidget::UpdateTyping()
{
	CurrentCharIndex++;
	FString CurrentString = FullString.Left(CurrentCharIndex);
	MessageText->SetText(FText::FromString(CurrentString));

	// I’[‚Ü‚Å’B‚µ‚½ê‡
	if (CurrentCharIndex >= FullString.Len())
	{
		SkipTyping();
	}
}


void ULinearDialogueWidget::SkipTyping()
{
	if (!bIsTyping) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TypewriterTimerHandle);
	}

	MessageText->SetText(FText::FromString(FullString));
	bIsTyping = false;
}
