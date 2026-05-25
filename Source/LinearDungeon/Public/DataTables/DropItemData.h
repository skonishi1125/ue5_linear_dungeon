#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Items/ItemBase.h"
#include "DropItemData.generated.h"

USTRUCT(BlueprintType)
struct LINEARDUNGEON_API FDropItemData : public FTableRowBase
{
	GENERATED_BODY()

	// ドロップするアイテムのクラス
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Data")
	TSubclassOf<AItemBase> ItemClass;

	// ドロップ確率 (0.0 = 0%, 1.0 = 100%)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Data", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DropRate = 0.2f;

	// 最小ドロップ数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Data", meta = (ClampMin = "1"))
	int32 MinQuantity = 1;

	// 最大ドロップ数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drop Data", meta = (ClampMin = "1"))
	int32 MaxQuantity = 1;

};