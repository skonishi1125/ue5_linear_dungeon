#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"

#include "LinearDialogueRow.generated.h"

// 選択肢のデータ（現状想定はないが、構造だけ作っておく）
USTRUCT(BlueprintType)
struct FLinearDialogueChoice
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FText ChoiceText;

	// 選択肢を選んだ時に遷移する DataTable の Row Name ( = ID )
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FName NextRowName;

};


USTRUCT(BlueprintType)
struct LINEARDUNGEON_API FLinearDialogueRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	FText SpeakerName; // ナレーターが話す場合、Null (空欄)にもなり得る

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	TArray<FText> DialogueLines;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dialogue")
	TArray<FLinearDialogueChoice> Choices;

};