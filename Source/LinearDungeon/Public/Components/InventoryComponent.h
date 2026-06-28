#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

// Potion ‚Ì”‚ª•Ï‚í‚Á‚½‚±‚Æ‚ð’Ê’m‚·‚é Delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNumOfPotionChangedDelegate, int32, NewNum);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LINEARDUNGEON_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

	void AddPotion();
	void UsePotion();
	FORCEINLINE int32 GetCurrentNumOfPotion() { return CurrentNumOfPotion; }
	FORCEINLINE void SetCurrentNumOfPotion(int32 Num) {  CurrentNumOfPotion = Num; }
	FORCEINLINE bool CanUsePotion() { return CurrentNumOfPotion > 0; }

	FOnNumOfPotionChangedDelegate NumOfPotionChanged;


protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category = "Items | Potion", meta = (AllowPrivateAccess = "true"))
	int32 CurrentNumOfPotion;

	UPROPERTY(EditAnywhere, Category = "Items | Potion", meta = (AllowPrivateAccess = "true"))
	int32 MaxPossessNumOfPotion = 9;

	UPROPERTY(EditAnywhere, Category = "Items | Potion", meta = (AllowPrivateAccess = "true"))
	float HealPointOfPotion = 50.f;

		
};
