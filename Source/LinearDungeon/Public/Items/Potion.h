#pragma once

#include "CoreMinimal.h"
#include "Items/ItemBase.h"
#include "Potion.generated.h"

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
	
};
