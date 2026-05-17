#include "Items/Weapon.h"
#include "Logging/StructuredLog.h"


void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	UE_LOGFMT(LogTemp, Warning, "Weapon BeginPlay");
}

void AWeapon::OnActivatedOverlap()
{
	//Super::OnActivatedOverlap();
	UE_LOGFMT(LogTemp, Warning, "AWeapon::OnActivatedOverlap()");
}

void AWeapon::OnDeactivatedOverlap()
{
	UE_LOGFMT(LogTemp, Warning, "AWeapon::OnDeactivatedOverlap()");
}
