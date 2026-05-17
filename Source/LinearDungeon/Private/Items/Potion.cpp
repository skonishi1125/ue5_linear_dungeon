#include "Items/Potion.h"
#include "Logging/StructuredLog.h"


void APotion::BeginPlay()
{
	Super::BeginPlay();
	UE_LOGFMT(LogTemp, Warning, "Potion BeginPlay");

}

void APotion::OnActivatedOverlap()
{
	UE_LOGFMT(LogTemp, Warning, "APotion::OnActivatedOverlap()");
}

void APotion::OnDeactivatedOverlap()
{
	UE_LOGFMT(LogTemp, Warning, "APotion::OnDeactivatedOverlap()");
}
