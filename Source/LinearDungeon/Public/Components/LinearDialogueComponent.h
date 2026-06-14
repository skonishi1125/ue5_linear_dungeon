#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LinearDialogueComponent.generated.h"

class UDataTable;
class ULinearDialogueWidget;

// セリフが完全に終了したことを通知するDelegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueFinishedSignature);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LINEARDUNGEON_API ULinearDialogueComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULinearDialogueComponent();

	// 渡された DataTable の Name から、テキスト配列を引っ張ってくる
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogueSequenceById(FName RowName);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void AdvanceDialogue();

	UPROPERTY(BlueprintAssignable, Category = "DIalogue")
	FOnDialogueFinishedSignature OnDialogueFinished;


protected:
	// 表示するWidgetのクラス指定
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
	TSubclassOf<ULinearDialogueWidget> DialogueWidgetClass;

	// 使用する会話情報が入った DataTable
	UPROPERTY(EditDefaultsOnly, Category = "Dialogue")
	TObjectPtr<UDataTable> DialogueDataTable;

private:
	void ShowNextDialogue();
	void EndDialogue();

	UPROPERTY()
	TObjectPtr<ULinearDialogueWidget> CurrentDialogueWidget;

	TArray<FText> DisplayingDialogueArray;
	int32 CurrentDialogueIndex = 0;

		
};
