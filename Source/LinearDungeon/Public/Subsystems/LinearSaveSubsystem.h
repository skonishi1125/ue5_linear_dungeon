#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LinearSaveSubsystem.generated.h"

class ULinearSaveGame;
class ALinearPlayerCharacter;

UCLASS()
class LINEARDUNGEON_API ULinearSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void SaveGame(ALinearPlayerCharacter* PlayerCharacter, int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "SaveSystem")
	void LoadGame(ALinearPlayerCharacter* PlayerCharacter, int32 SlotIndex);
	
private:
	// å≈íËÇÃÉXÉçÉbÉgñº
	const FString BaseSaveSlotName = TEXT("SaveSlot_");
};
