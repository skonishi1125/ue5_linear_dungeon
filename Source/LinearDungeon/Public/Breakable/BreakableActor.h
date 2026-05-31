#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;
class UCapsuleComponent;
class UChaosGameplayEventDispatcher;

// drop item
class ULootDropComponent;

// 物理的に壊れた場合のイベント 使用のための宣言
struct FChaosBreakEvent;

UCLASS()
class LINEARDUNGEON_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;
	// Interface の Override
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnChaosBreakEventBreakable(const FChaosBreakEvent& BreakEvent);

	UFUNCTION(BlueprintImplementableEvent, Category = "Breakable")
	void OnBroken();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGeometryCollectionComponent> GeometryCollection;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UChaosGameplayEventDispatcher> GameplayEventDispatcher;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* Capsule;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drop Items", meta = (AllowPrivateAccess = "true"))
	//TSubclassOf<AItemBase> DropItemClassToSpawn;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULootDropComponent> LootDropComponent;

	bool bIsBroken = false;
};
