#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LinearSequenceSubsystem.generated.h"

UCLASS()
class LINEARDUNGEON_API ULinearSequenceSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	FORCEINLINE bool IsPlayingSequence() { return bIsPlayingSequence; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE void SetIsPlayingSequence(bool bIsPlay) { bIsPlayingSequence = bIsPlay; }

private:
	bool bIsPlayingSequence = false;
	
};
