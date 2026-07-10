#include "Items/Potion.h"
#include "Logging/StructuredLog.h"

#include "Characters/LinearPlayerCharacter.h"
#include "Components/InventoryComponent.h"
#include "Components/SphereComponent.h"

// ‰¹
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"


void APotion::BeginPlay()
{
	Super::BeginPlay();

	if (OverlapSphere && PickupActivationDelay > 0.f)
	{
		OverlapSphere->SetGenerateOverlapEvents(false);
		GetWorldTimerManager().SetTimer(
			PickupEnableTimer, this, &APotion::EnablePickup, PickupActivationDelay, false
		);
	}
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
			if (GetSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, GetSound, GetActorLocation());
			}
			Destroy();
		}
	}
}

void APotion::EnablePickup()
{
	if (OverlapSphere)
	{
		OverlapSphere->SetGenerateOverlapEvents(true);
		// Šù‚ÉƒvƒŒƒCƒ„[‚ªã‚Éæ‚Á‚Ä‚¢‚éê‡‚Å‚à BeginOverlap ‚ðÄ”­‰Î
		OverlapSphere->UpdateOverlaps();
	}
}
