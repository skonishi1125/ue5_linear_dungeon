#include "Items/Potion.h"
#include "Logging/StructuredLog.h"


void APotion::BeginPlay()
{
	Super::BeginPlay();
	UE_LOGFMT(LogTemp, Warning, "Potion BeginPlay");

}

void APotion::OnItemBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnItemBeginOverlap(
		OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult
	);
	UE_LOGFMT(LogTemp, Warning, "APotion::OnItemBeginOverlap");
}

void APotion::OnItemEndOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex
)
{
	Super::OnItemEndOverlap(
		OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex
	);
	UE_LOGFMT(LogTemp, Warning, "APotion::OnItemEndOverlap");
}
