#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MovingDoorInterface.generated.h"

// EncounterManager などから、閉じているドアを開けるときに呼ぶ
UINTERFACE(MinimalAPI, Blueprintable)
class UMovingDoorInterface : public UInterface
{
	GENERATED_BODY()
};

class LINEARDUNGEON_API IMovingDoorInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Encounter|Door")
	void PlayOpen(bool bInstant); // false: 演出再生 true: 即時削除(ロード時など)

};
