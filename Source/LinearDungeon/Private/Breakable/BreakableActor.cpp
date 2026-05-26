#include "Breakable/BreakableActor.h"
#include "Logging/StructuredLog.h"

#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/CapsuleComponent.h"
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

	// 破片が Pawn に干渉しないように設定
	// GC 自体の Collision は遮断して、CapsuleComponent 側で Block する形をとる
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);


	// Event Dispatcher 設定
	GameplayEventDispatcher = CreateDefaultSubobject<UChaosGameplayEventDispatcher>(TEXT("GameplayEventDispatcher"));

	// 固有 Component 割当（アイテムドロップ）
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

