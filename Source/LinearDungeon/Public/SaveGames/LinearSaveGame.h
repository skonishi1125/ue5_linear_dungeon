#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LinearSaveGame.generated.h"

UCLASS()
class LINEARDUNGEON_API ULinearSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
    // プレイヤーの座標・回転・スケール
    UPROPERTY(BlueprintReadWrite, Category = "SaveData")
    FTransform PlayerTransform;

    // プレイヤーの体力
    UPROPERTY(BlueprintReadWrite, Category = "SaveData")
    float PlayerHealth;

    // セーブ日時
    UPROPERTY(VisibleAnywhere, Category = "SaveData")
    FDateTime SaveDate;
	
};
