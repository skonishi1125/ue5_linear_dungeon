// Copyright Epic Games, Inc. All Rights Reserved.

#include "LinearDungeonGameMode.h"
#include "LinearDungeonCharacter.h"
#include "UObject/ConstructorHelpers.h"

ALinearDungeonGameMode::ALinearDungeonGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
