#include "Breakable/BreakableActor.h"
#include "Logging/StructuredLog.h"

#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
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
	// GC 自体の Collision は遮断して、BoxCollision 側で Block する形をとる
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	// GC 同士（飛んだ破片が別の GC を倒すのを防ぐ）
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetNotifyBreaks(true);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(GetRootComponent());
	BoxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);


	// Event Dispatcher 設定
	GameplayEventDispatcher = CreateDefaultSubobject<UChaosGameplayEventDispatcher>(TEXT("GameplayEventDispatcher"));

	// 固有 Component 割当（アイテムドロップ）
	LootDropComponent = CreateDefaultSubobject<ULootDropComponent>(TEXT("LootDropComponent"));
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();

	if (GeometryCollection)
	{
		GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::OnChaosBreakEventBreakable);
	}
}

// Weapon, Rolling などで物が壊れたときの共通処理
void ABreakableActor::OnChaosBreakEventBreakable(const FChaosBreakEvent& BreakEvent)
{
	if (bIsBroken) return;
	bIsBroken = true;

	// アイテムドロップ 処理
	if (LootDropComponent)
	{
		// 自身の位置を渡して、ドロップ処理自体はコンポーネントに委譲
		LootDropComponent->ExecuteDrop(GetActorLocation(), GetActorRotation());
	}

	// BP 側で、Actor ごとの固有処理
	// LifeSpan の設定（C++ でも書けるがデザイナーが弄る想定として、あちら側で書く）
	// Pot なら壺が割れる音、シャンデリアなら 崩れ落ちる音などの再生
	OnBroken();
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Weapon から GetHit を呼ぶ時の固有仕様設定用
void ABreakableActor::GetHit_Implementation(
	const FVector& ImpactPoint, const float PoiseDamage
)
{
	UE_LOGFMT(LogTemp, Warning, " ABreakableActor::GetHit_Implementation()");
	//if (DropItemClassToSpawn && GetWorld())
	//{
	//	const FVector SpawnLocation = GetActorLocation();
	//	const FRotator SpawnRotation = GetActorRotation();
	//	GetWorld()->SpawnActor<AItemBase>(DropItemClassToSpawn, SpawnLocation, SpawnRotation);
	//	UE_LOGFMT(LogTemp, Warning, "アイテム生成");
	//}
}

