#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractInterface.generated.h"

// This class does not need to be modified.
// BP からでも Interface を 始点ノードとして書けるようにする
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
};
