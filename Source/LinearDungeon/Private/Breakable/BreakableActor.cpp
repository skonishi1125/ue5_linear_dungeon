#include "Breakable/BreakableActor.h"
#include "Logging/StructuredLog.h"

#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Chaos/ChaosGameplayEventDispatcher.h"

//#include "Items/ItemBase.h"
#include "Components/LootDropComponent.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetNotifyBreaks(true);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(GetRootComponent());
	BoxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);


	GameplayEventDispatcher = CreateDefaultSubobject<UChaosGameplayEventDispatcher>(TEXT("GameplayEventDispatcher"));

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

void ABreakableActor::OnChaosBreakEventBreakable(const FChaosBreakEvent& BreakEvent)
{
	if (bIsBroken) return;
	bIsBroken = true;

	if (LootDropComponent)
	{
		LootDropComponent->ExecuteDrop(GetActorLocation(), GetActorRotation());
	}

	OnBroken();
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit_Implementation(
	const FVector& ImpactPoint, const float PoiseDamage, bool bIsParry
)
{
	UE_LOGFMT(LogTemp, Warning, " ABreakableActor::GetHit_Implementation()");
}

