#include "Breakable/BreakableActor.h"
#include "Logging/StructuredLog.h"

#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Chaos/ChaosGameplayEventDispatcher.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	GameplayEventDispatcher = CreateDefaultSubobject<UChaosGameplayEventDispatcher>(TEXT("GameplayEventDispatcher"));
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableActor::GetHit(const FVector& ImpactPoint)
{
	UE_LOGFMT(LogTemp, Warning, " ABreakableActor::GetHit");
}

