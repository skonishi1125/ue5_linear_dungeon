#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LinearDungeonHUD.generated.h"

class ULinearDungeonOverlay;

UCLASS()
class LINEARDUNGEON_API ALinearDungeonHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULinearDungeonOverlay> LinearDungeonOverlayClass;

	// ダメージを受けたときなど、この Overlay 経由で HUD のパラメータを調整することになる
	UPROPERTY()
	TObjectPtr<ULinearDungeonOverlay> Overlay;
	
};
