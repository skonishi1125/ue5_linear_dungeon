#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;
class UChaosGameplayEventDispatcher;

// drop item
class ULootDropComponent;

UCLASS()
class LINEARDUNGEON_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;

	// Interface ÇÃ Override
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

protected:
	virtual void BeginPlay() override;

private:
	// BP Ç≈âÛÇÍÇΩÇ∆Ç´Ç…éQè∆Ç∑ÇÈÇÊÇ§Ç…ÇµÇΩÇ©ÇÁÅAVisibleAnywhere Ç©ÇÁïœÇ¶ÇΩ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGeometryCollectionComponent> GeometryCollection;

	UPROPERTY()
	TObjectPtr<UChaosGameplayEventDispatcher> GameplayEventDispatcher;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drop Items", meta = (AllowPrivateAccess = "true"))
	//TSubclassOf<AItemBase> DropItemClassToSpawn;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULootDropComponent> LootDropComponent;
};
