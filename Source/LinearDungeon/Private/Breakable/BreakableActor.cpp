#include "Breakable/BreakableActor.h"
#include "Logging/StructuredLog.h"

#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Chaos/ChaosGameplayEventDispatcher.h"

#include "Items/ItemBase.h"
#include "Components/LootDropComponent.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	GameplayEventDispatcher = CreateDefaultSubobject<UChaosGameplayEventDispatcher>(TEXT("GameplayEventDispatcher"));

	// Component 割当
	LootDropComponent = CreateDefaultSubobject<ULootDropComponent>(TEXT("LootDropComponent"));
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(const FVector& ImpactPoint)
{
	UE_LOGFMT(LogTemp, Warning, " ABreakableActor::GetHit_Implementation()");
	//if (DropItemClassToSpawn && GetWorld())
	//{
	//	const FVector SpawnLocation = GetActorLocation();
	//	const FRotator SpawnRotation = GetActorRotation();
	//	GetWorld()->SpawnActor<AItemBase>(DropItemClassToSpawn, SpawnLocation, SpawnRotation);
	//	UE_LOGFMT(LogTemp, Warning, "アイテム生成");
	//}
	if (LootDropComponent)
	{
		// 自身の位置を渡してドロップ処理をコンポーネントに委譲
		LootDropComponent->ExecuteDrop(GetActorLocation(), GetActorRotation());
	}
}

