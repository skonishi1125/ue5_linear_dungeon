#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"

#include "LinearSaveGame.generated.h"

class AWeapon;
class AShield;

UCLASS()
class LINEARDUNGEON_API ULinearSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
    // プレイヤーの座標・回転・スケール
    UPROPERTY(BlueprintReadWrite, Category = "SaveData")
    FTransform PlayerTransform;

    // 装備武器 | 盾
    UPROPERTY(BlueprintReadWrite, Category = "SaveData")
    TSubclassOf<AWeapon> EquippedWeaponClass;
    UPROPERTY(BlueprintReadWrite, Category = "SaveData")
    TSubclassOf<AShield> EquippedShieldClass;

    // プレイヤーの体力
    UPROPERTY(BlueprintReadWrite, Category = "SaveData")
    float PlayerHealth;

    // ポーションの数
    UPROPERTY(BlueprintReadWrite, Category = "SaveData")
    int32 NumOfPotion;

    // セーブ日時
    UPROPERTY(VisibleAnywhere, Category = "SaveData")
    FDateTime SaveDate;
	
    // セーブした場所の Level 名
    UPROPERTY(BlueprintReadWrite, Category = "SaveData")
    FString SavedLevelName;

    // クリア済みイベントの ID を保存するリスト
    UPROPERTY(BlueprintReadWrite, Category = "SaveData|Events")
    TSet<FName> ClearedEvents;

    // 汎用進行フラグ（敵の全滅、宝箱の取得、NPCの会話状況など）を保存するリスト
    // TSet: 重複登録を防ぐ, Contains などが高速に行える
    UPROPERTY(BlueprintReadWrite, Category = "SaveData|Flags")
    TSet<FName> SavedFlags;

};
