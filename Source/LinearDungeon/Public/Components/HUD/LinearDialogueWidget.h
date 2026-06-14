#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LinearDialogueWidget.generated.h"

class UTextBlock;

UCLASS()
class LINEARDUNGEON_API ULinearDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void StartDialogueText(const FText& DisplayText);
	void SkipTyping(); // 1文字ずつ出てくる描写をスキップして、全文を出す
	FORCEINLINE bool IsTyping() const { return bIsTyping; }

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MessageText;

	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
	float TypewriterSpeed = 0.05f;

private:
	void UpdateTyping();

	FTimerHandle TypewriterTimerHandle;
	FString RawTextFString;
	int32 CurrentCharIndex = 0;
	bool bIsTyping = false;
	
};
