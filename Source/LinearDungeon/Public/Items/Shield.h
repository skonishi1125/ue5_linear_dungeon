#pragma once

#include "CoreMinimal.h"
#include "Items/ItemBase.h"
#include "Interfaces/InteractInterface.h"

#include "Shield.generated.h"

class USoundBase; // UMetaSound ÇÕÇ±ÇÍÇåpè≥Ç∑ÇÈÇÃÇ≈ÅAÇ±ÇÃÉNÉâÉXÇ∆ÇµÇƒäiî[Ç≈Ç´ÇÈ
class UBoxComponent;

UCLASS()
class LINEARDUNGEON_API AShield : public AItemBase, public IInteractInterface
{
	GENERATED_BODY()

public:
	// InteractInterface ä÷òA
	virtual void Interact_Implementation(AActor* InstigatorActor) override;
	virtual FText GetInteractPrompt_Implementation() override;

	void Equip(
		USceneComponent* InParent, FName InSocketName,
		AActor* NewOwner, APawn* NewInstigator, bool bPlaySound = true
	);

	// èÇÇéÃÇƒÇÈèàóù
	UFUNCTION()
	void Drop(const FVector& DropLocation);

protected:
	virtual void OnItemBeginOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult
	) override;

	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex
	) override;

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<USoundBase> EquipSound;

};
