#include "Items/Weapon.h"
#include "Logging/StructuredLog.h"


void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	UE_LOGFMT(LogTemp, Warning, "Weapon BeginPlay");
}

void AWeapon::OnActivated()
{
	Super::OnActivated();
	UE_LOGFMT(LogTemp, Warning, "AWeapon::OnActivated()");
}
