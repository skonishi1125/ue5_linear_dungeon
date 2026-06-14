#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LinearDialogueComponent.generated.h"

class ULinearDialogueWidget;

// セリフが完全に終了したことを通知するDelegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueFinishedSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LINEARDUNGEON_API ULinearDialogueComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULinearDialogueComponent();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogueSequence(const TArray<FText>& PassedDialogueArray);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void AdvanceDialogue();

	UPROPERTY(BlueprintAssignable, Category = "DIalogue")
	FOnDialogueFinishedSignature OnDialogueFinished;


protected:
	// 表示するWidgetのクラス指定
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
	TSubclassOf<ULinearDialogueWidget> DialogueWidgetClass;

private:
	void ShowNextDialogue();
	void EndDialogue();

	UPROPERTY()
	TObjectPtr<ULinearDialogueWidget> CurrentDialogueWidget;

	TArray<FText> DisplayingDialogueArray;
	int32 CurrentDialogueIndex = 0;

		
};
