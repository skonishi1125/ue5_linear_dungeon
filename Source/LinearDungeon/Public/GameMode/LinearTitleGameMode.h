#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LinearTitleGameMode.generated.h"

class USoundBase;

UCLASS()
class LINEARDUNGEON_API ALinearTitleGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Audio")
	TObjectPtr<USoundBase> TitleBGM;
	
};
