#include "Components/LinearDialogueComponent.h"
#include "Components/HUD/LinearDialogueWidget.h"
#include "Characters/LinearPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

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

	// 会話開始時、Player に対して自信を登録して、会話中状態にする
	// movie シーンなどであっても同様。
	ALinearPlayerCharacter* Player = Cast<ALinearPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Player)
	{
		Player->SetActiveDialogueComponent(this);
		Player->SetCharacterActionState(EActionState::EAS_InDialogue);

		// 終了時の通知を登録
		OnDialogueFinished.AddUniqueDynamic(Player, &ALinearPlayerCharacter::OnDialogueEnd);
	}

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

	// プレイヤーの参照解除
	ALinearPlayerCharacter* Player = Cast<ALinearPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Player)
	{
		Player->SetActiveDialogueComponent(nullptr);
	}

	// 外部（Level Blueprint や Sequencer 等）に終了を通知
	OnDialogueFinished.Broadcast();
}
