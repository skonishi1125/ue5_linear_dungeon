#include "Components/LootDropComponent.h"
#include "Logging/StructuredLog.h"

#include "Engine/DataTable.h"
#include "DataTables/DropItemData.h"

// Sets default values for this component's properties
ULootDropComponent::ULootDropComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


void ULootDropComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULootDropComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void ULootDropComponent::ExecuteDrop(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	UE_LOGFMT(LogTemp, Warning, "ULootDropComponent::ExecuteDrop()");

	if (!LootTable || !GetWorld()) return;

	if (bIsExecDrop == true)
	{
		UE_LOGFMT(LogTemp, Warning, " This Item already executedrop().");
		return;
	}

	bIsExecDrop = true;

	TArray<FDropItemData*> AllRows;
	LootTable->GetAllRows<FDropItemData>(TEXT("LootDropContext"), AllRows);


	for (FDropItemData* Row : AllRows)
	{
		if (Row && Row->ItemClass)
		{
			const float RandomValue = FMath::FRand();

			if (RandomValue <= Row->DropRate)
			{
				// ドロップ数を増やすなら、 FMath::RandRange(Row->MinQuantity, Row->MaxQuantity) で指定する
				UE_LOGFMT(LogTemp, Warning, "Item spawned.");
				const int32 SpawnCount = 1;
				GetWorld()->SpawnActor<AItemBase>(Row->ItemClass, SpawnLocation, SpawnRotation);
			}
			else
			{
				UE_LOGFMT(LogTemp, Warning, "Item doesn't spawned.");
			}
		}
	}

}

