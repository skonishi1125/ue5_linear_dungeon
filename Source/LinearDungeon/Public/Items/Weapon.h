#pragma once

#include "CoreMinimal.h"
#include "Items/ItemBase.h"
#include "Weapon.generated.h"

UCLASS()
class LINEARDUNGEON_API AWeapon : public AItemBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// 親側で UFUNCTION() を付与していたら、書かなくてよい
	// アクセス権は親が protected なので、そちらに合わせるのが基本的な考え方
	virtual void OnItemBeginOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult
	) override;

	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex
	) override;

};
