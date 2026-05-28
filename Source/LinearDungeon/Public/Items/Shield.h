#pragma once

#include "CoreMinimal.h"
#include "Items/ItemBase.h"
#include "Shield.generated.h"

class USoundBase; // UMetaSound ÇÕÇ±ÇÍÇåpè≥Ç∑ÇÈÇÃÇ≈ÅAÇ±ÇÃÉNÉâÉXÇ∆ÇµÇƒäiî[Ç≈Ç´ÇÈ
class UBoxComponent;

UCLASS()
class LINEARDUNGEON_API AShield : public AItemBase
{
	GENERATED_BODY()

public:
	void Equip(
		USceneComponent* InParent, FName InSocketName,
		AActor* NewOwner, APawn* NewInstigator
	);
private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<USoundBase> EquipSound;

};
