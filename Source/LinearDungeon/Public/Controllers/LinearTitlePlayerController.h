#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LinearTitlePlayerController.generated.h"

UCLASS()
class LINEARDUNGEON_API ALinearTitlePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
};
