#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

// 会話関連の Interface
// 現状 BP_Prisoner という BP Class だけを想定して書いている
// そのため、C++ Class を持つものに対して使う場合は設計を見直す必要があるかも。
// Blueprintable で BP からでも Interface を始点ノードとして書けるようにする
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractInterface : public UInterface
{
	GENERATED_BODY()
};

class LINEARDUNGEON_API IInteractInterface
{
	GENERATED_BODY()

public:
	// 実行対象（大抵はPlayer）を引数として受け取る
	// BP で呼べるようにしておく
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* InstigatorActor);

	// InteractMarker に書き込む文字列を設定して返す
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractPrompt() const;

};
