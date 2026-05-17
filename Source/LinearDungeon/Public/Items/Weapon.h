#pragma once

#include "CoreMinimal.h"
#include "Items/ItemBase.h"
#include "Weapon.generated.h"

UCLASS()
class LINEARDUNGEON_API AWeapon : public AItemBase
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	virtual void OnActivated() override;

};
