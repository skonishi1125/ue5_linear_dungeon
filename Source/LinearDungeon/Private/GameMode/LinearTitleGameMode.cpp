#include "GameMode/LinearTitleGameMode.h"
#include "Sound/SoundBase.h"
#include "Subsystems/LinearAudioSubsystem.h"

void ALinearTitleGameMode::BeginPlay()
{
	Super::BeginPlay();
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULinearAudioSubsystem* AudioSubsystem = GI->GetSubsystem<ULinearAudioSubsystem>())
		{
			AudioSubsystem->PlayBGM(TitleBGM);
		}
	}
}