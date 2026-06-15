#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DialogueEventSubsystem.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGlobalDialogueEventSignature);

UCLASS()
class LINEARDUNGEON_API UDialogueEventSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	// 会話開始時に呼ばれるイベント
	UPROPERTY(BlueprintAssignable, Category = "Dialogue|Event")
	FOnGlobalDialogueEventSignature OnDialogueStarted;

	// 会話終了時に呼ばれるイベント
	UPROPERTY(BlueprintAssignable, Category = "Dialogue|Event")
	FOnGlobalDialogueEventSignature OnDialogueFinished;
};
