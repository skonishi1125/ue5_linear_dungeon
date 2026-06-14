#include "Components/LinearDialogueComponent.h"
#include "Components/HUD/LinearDialogueWidget.h"
#include "Characters/LinearPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

ULinearDialogueComponent::ULinearDialogueComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// 会話処理の初期設定
// 表示すべきテキスト情報の配列を受け取り、ダイアログ Widget 自体の生成なども行う
void ULinearDialogueComponent::StartDialogueSequence(const TArray<FText>& PassedDialogueArray)
{
	if (PassedDialogueArray.IsEmpty() || !DialogueWidgetClass) return;

	DisplayingDialogueArray = PassedDialogueArray;
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

	// 会話開始時、Player のクラス変数に自分を登録
	// これで、会話を進めたい時は Player が ボタンを押す -> Component->AdvancedDialogue() が呼べる形になる
	ALinearPlayerCharacter* Player = Cast<ALinearPlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if (Player)
	{
		Player->SetActiveDialogueComponent(this);
		Player->SetCharacterActionState(EActionState::EAS_InDialogue);

		// 終了時の通知を登録
		OnDialogueFinished.AddUniqueDynamic(Player, &ALinearPlayerCharacter::OnDialogueEnd);
	}

	// [0] つめの会話（最初の会話）を出す
	ShowNextDialogue();
}

void ULinearDialogueComponent::ShowNextDialogue()
{
	// 全てのページを表示し終えた場合、処理終了
	if (CurrentDialogueIndex >= DisplayingDialogueArray.Num())
	{
		EndDialogue();
		return;
	}

	if (CurrentDialogueWidget)
	{
		CurrentDialogueWidget->StartDialogueText(DisplayingDialogueArray[CurrentDialogueIndex]);
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


void ULinearDialogueComponent::EndDialogue()
{
	if (CurrentDialogueWidget)
	{
		CurrentDialogueWidget->RemoveFromParent();
	}

	DisplayingDialogueArray.Empty();
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
