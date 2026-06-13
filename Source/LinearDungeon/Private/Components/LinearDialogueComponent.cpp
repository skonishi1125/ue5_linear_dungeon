#include "Components/LinearDialogueComponent.h"
#include "Components/HUD/LinearDialogueWidget.h"
//#include "Blueprint/UserWidget.h"

ULinearDialogueComponent::ULinearDialogueComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void ULinearDialogueComponent::StartDialogueSequence(const TArray<FText>& InDialogues)
{
	if (InDialogues.IsEmpty() || !DialogueWidgetClass) return;

	DialogueQueue = InDialogues;
	CurrentDialogueIndex = 0;

	// Widgetがまだ生成されていなければ生成する
	if (!CurrentDialogueWidget)
	{
		CurrentDialogueWidget = CreateWidget<ULinearDialogueWidget>(GetWorld(), DialogueWidgetClass);
	}

	if (CurrentDialogueWidget && !CurrentDialogueWidget->IsInViewport())
	{
		CurrentDialogueWidget->AddToViewport();
	}

	ShowNextDialogue();
}

void ULinearDialogueComponent::AdvanceDialogue()
{
	if (!CurrentDialogueWidget) return;

	// タイピング中ならスキップして全表示
	if (CurrentDialogueWidget->IsTyping())
	{
		CurrentDialogueWidget->SkipTyping();
	}
	else
	{
		// 表示完了済みなら次のページへ
		CurrentDialogueIndex++;
		ShowNextDialogue();
	}
}

void ULinearDialogueComponent::ShowNextDialogue()
{
	// 全てのページを表示し終えた場合
	if (CurrentDialogueIndex >= DialogueQueue.Num())
	{
		EndDialogue();
		return;
	}

	if (CurrentDialogueWidget)
	{
		CurrentDialogueWidget->StartDialogueText(DialogueQueue[CurrentDialogueIndex]);
	}
}

void ULinearDialogueComponent::EndDialogue()
{
	if (CurrentDialogueWidget)
	{
		CurrentDialogueWidget->RemoveFromParent();
	}

	DialogueQueue.Empty();
	CurrentDialogueIndex = 0;

	// 外部（Level BlueprintやSequencer等）に終了を通知
	OnDialogueFinished.Broadcast();
}
