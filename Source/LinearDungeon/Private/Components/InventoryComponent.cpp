#include "Components/InventoryComponent.h"
#include "Logging/StructuredLog.h"

#include "Components/AttributeComponent.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UInventoryComponent::AddPotion()
{
	// Å‘åŠ”
	if (CurrentNumOfPotion == MaxPossessNumOfPotion) return;

	CurrentNumOfPotion += 1;
	NumOfPotionChanged.Broadcast(CurrentNumOfPotion);

}

void UInventoryComponent::UsePotion()
{
	if (CanUsePotion())
	{
		CurrentNumOfPotion -= 1;

		// ‘Ì—Í‰ñ•œ
		AActor* OwnerActor = GetOwner();
		if (OwnerActor)
		{
			UAttributeComponent* Attributes = Cast<UAttributeComponent>(
				OwnerActor->GetComponentByClass(UAttributeComponent::StaticClass())
			);
			if (Attributes)
			{
				Attributes->ReceiveHealthHeal(HealPointOfPotion);
			}
		}

		NumOfPotionChanged.Broadcast(CurrentNumOfPotion);
	}
}
