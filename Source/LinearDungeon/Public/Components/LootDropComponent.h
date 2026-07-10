#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LootDropComponent.generated.h"

class UDataTable;
class AItemBase;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LINEARDUNGEON_API ULootDropComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULootDropComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Loot")
	void ExecuteDrop(const FVector& SpawnLocation, const FRotator& SpawnRotation);


protected:
	virtual void BeginPlay() override;

	// DataTable を使った形だが、現状ポーションしか無く過剰なので使わないことにする
	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Loot")
	//TObjectPtr<UDataTable> LootTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	TSubclassOf<AItemBase> DropItemClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DropRate = 1.0f;

	bool bIsExecDrop = false;
		
};
