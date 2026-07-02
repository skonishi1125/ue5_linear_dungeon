#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LinearDungeonHUD.generated.h"

class ULinearDungeonOverlay;

UCLASS()
class LINEARDUNGEON_API ALinearDungeonHUD : public AHUD
{
	GENERATED_BODY()

public:
	FORCEINLINE ULinearDungeonOverlay* GetOverlay() const { return Overlay; }

	// LevelSequence 中、画面左上の UI を切るための関数
	UFUNCTION(BlueprintCallable)
	void SetOverlayVisibility(bool bIsVisible);

protected:
	virtual void BeginPlay() override;

	// レベル開始時にHUDを非表示状態にするかどうかのフラグ
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	bool bStartHidden = false;

private:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ULinearDungeonOverlay> LinearDungeonOverlayClass;

	// ダメージを受けたときなど、この Overlay 経由で HUD のパラメータを調整することになる
	UPROPERTY()
	TObjectPtr<ULinearDungeonOverlay> Overlay;

};
