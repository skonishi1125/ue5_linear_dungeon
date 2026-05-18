#include "Items/Weapon.h"
#include "Logging/StructuredLog.h"

#include "Characters/LinearPlayerCharacter.h"


void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	UE_LOGFMT(LogTemp, Warning, "Weapon BeginPlay");
}

void AWeapon::OnItemBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnItemBeginOverlap(
		OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult
	);
	UE_LOGFMT(LogTemp, Warning, "AWeapon::OnItemBeginOverlap");
}

void AWeapon::OnItemEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnItemEndOverlap(
		OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex
	);
	UE_LOGFMT(LogTemp, Warning, "AWeapon::OnItemEndOverlap");

}


