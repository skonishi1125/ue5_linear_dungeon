#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SaveInterface.generated.h"

class ULinearSaveGame;

UINTERFACE(MinimalAPI)
class USaveInterface : public UInterface
{
	GENERATED_BODY()
};

// セーブしたい Actor などに付与する
class LINEARDUNGEON_API ISaveInterface
{
	GENERATED_BODY()

public:
	// {} で、純粋仮想関数ではなく、空のデフォルト実装にしておく
	// （他アクターに実装したとき、実装が必須にならないようにする）
	virtual void OnSaveGame(ULinearSaveGame* SaveGameObj) {}
	virtual void OnLoadGame(ULinearSaveGame* SaveGameObj) {}

};
