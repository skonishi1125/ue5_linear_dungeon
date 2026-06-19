#include "GameMode/LinearGameMode.h"
#include "Sound/SoundBase.h"
#include "Subsystems/LinearAudioSubsystem.h"

// ゲーム開始時に鳴らす BGM などは、
// BP_LinearGameMode の BeginPlay Node などで下記を呼び出せばよい。

void ALinearGameMode::ChangeBGM(EBGMType BGMType)
{
    // TMap の中に指定された Enum のキーが存在するかチェック
    if (BGMList.Contains(BGMType))
    {
        USoundBase* SoundToPlay = BGMList[BGMType];

        // Subsystem を取得して再生
        ULinearAudioSubsystem* AudioSubsystem = GetGameInstance()->GetSubsystem<ULinearAudioSubsystem>();
        if (AudioSubsystem)
        {
            AudioSubsystem->PlayBGM(SoundToPlay);
        }
    }
}
