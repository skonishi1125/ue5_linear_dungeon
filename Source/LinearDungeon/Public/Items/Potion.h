#pragma once

#include "CoreMinimal.h"
#include "Items/ItemBase.h"

#include "Potion.generated.h"

class USoundBase; // MetaSound 鳴らす用

UCLASS()
class LINEARDUNGEON_API APotion : public AItemBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void OnItemBeginOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult
	) override;

private:
	UPROPERTY(EditAnywhere, Category = "Properties")
	TObjectPtr<USoundBase> GetSound;

	// アイテムが拾えるようになるまでの時間
	// 壺などをローリングで割った時、即取得になり取れたか分かりづらいのでディレイをかける
	UPROPERTY(EditAnywhere, Category = "Potion")
	float PickupActivationDelay = 0.7f;

	void EnablePickup();
	FTimerHandle PickupEnableTimer;
	
};
