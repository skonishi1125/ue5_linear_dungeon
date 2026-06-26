#include "Items/Potion.h"
#include "Logging/StructuredLog.h"

#include "Characters/LinearPlayerCharacter.h"
#include "Components/InventoryComponent.h"


void APotion::BeginPlay()
{
	Super::BeginPlay();
}

void APotion::OnItemBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnItemBeginOverlap(
		OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult
	);

	// Player ‚ª Potion ‚ÉG‚ê‚½‚Æ‚«
	if (OtherActor->ActorHasTag(ALinearPlayerCharacter::GetTag()))
	{
		UInventoryComponent* Inventories = Cast<UInventoryComponent>(
			OtherActor->GetComponentByClass(UInventoryComponent::StaticClass())
		);

		if (Inventories)
		{
			Inventories->AddPotion();
			// TODO: Œø‰Ê‰¹–Â‚ç‚·
			Destroy();
		}
	}



}
