#include "Items/Potion.h"
#include "Logging/StructuredLog.h"


void APotion::BeginPlay()
{
	Super::BeginPlay();
	UE_LOGFMT(LogTemp, Warning, "Potion BeginPlay");

}

void APotion::OnActivated()
{
	UE_LOGFMT(LogTemp, Warning, "APotion::OnActivated()");
}
