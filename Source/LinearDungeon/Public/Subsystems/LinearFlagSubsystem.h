#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include "LinearFlagSubsystem.generated.h"

UCLASS()
class LINEARDUNGEON_API ULinearFlagSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "GameFlags")
	void SetFlag(FName FlagName);

	UFUNCTION(BlueprintCallable, Category = "GameFlags")
	bool CheckFlag(FName FlagName) const;

	// セーブデータ書込用 現在の全フラグを取得
	const TSet<FName>& GetAllFlags() const { return ActiveFlags; }

	// セーブデータ読込用 フラグ一覧を復元する
	void RestoreAllFlags(const TSet<FName>& InSavedFlags);

private:
	TSet<FName> ActiveFlags;
	
};
