#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LinearSettingsSaveGame.generated.h"

UCLASS()
class LINEARDUNGEON_API ULinearSettingsSaveGame : public USaveGame
{
	GENERATED_BODY()

// •Û‘¶‚µ‚½‚¢İ’è‚ÍA‚Ç‚±‚©‚ç‚Å‚à“Ç‚İ‘‚«‚Å‚«‚é‚æ‚¤‚É public ‚Æ‚µ‚Ä‚¨‚­
public:
	UPROPERTY()
	float MouseSensitivity = 1.0f;

	UPROPERTY()
	float BGMVolume = 1.0f;

	
};
