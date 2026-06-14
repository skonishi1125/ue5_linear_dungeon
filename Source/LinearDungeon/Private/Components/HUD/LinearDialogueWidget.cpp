#include "Components/HUD/LinearDialogueWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

// Component から呼ぶ想定の、渡された会話を画面に出す処理
// 
void ULinearDialogueWidget::StartDialogueText(const FText& DisplayText)
{
	if (!MessageText) return;

	// 会話を生成するとき、タイピング用関数の Timer をリセットしておく
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TypewriterTimerHandle);
	}

	// FText は 1文字ずつ切り出せないので、FString にして処理を行う
	RawTextFString = DisplayText.ToString();
	CurrentCharIndex = 0;
	bIsTyping = true;

	// 空文字列をセットして、何も文字が表示されていない初期状態を作る
	MessageText->SetText(FText::GetEmpty());

	// タイピング処理
	if (UWorld* World = GetWorld())
	{
		// UpdateTyping() を TypeWriterSpeed の頻度で ループさせる
		World->GetTimerManager().SetTimer(
			TypewriterTimerHandle, this, &ULinearDialogueWidget::UpdateTyping, TypewriterSpeed,	true
		);
	}
}


void ULinearDialogueWidget::UpdateTyping()
{
	CurrentCharIndex++;
	// index 分だけ文字列から切り出し、MessageText に反映（反映時は FString -> FText に戻す）
	FString CurrentString = RawTextFString.Left(CurrentCharIndex);
	MessageText->SetText(FText::FromString(CurrentString));

	// 終端まで達した場合
	if (CurrentCharIndex >= RawTextFString.Len())
	{
		SkipTyping();
	}
}

// 1文字ずつ表示させる挙動の終了処理
void ULinearDialogueWidget::SkipTyping()
{
	if (!bIsTyping) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TypewriterTimerHandle);
	}

	MessageText->SetText(FText::FromString(RawTextFString));
	bIsTyping = false;
}
