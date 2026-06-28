#include "GameMode/LinearGameMode.h"
#include "Logging/StructuredLog.h"

// 音
#include "Sound/SoundBase.h"
#include "Subsystems/LinearAudioSubsystem.h"

// Save
#include "Subsystems/LinearSaveSubsystem.h"

// ゲーム開始時に鳴らす BGM などは、
// BP_LinearGameMode の BeginPlay Node などで下記を呼び出している
void ALinearGameMode::BeginPlay()
{
    Super::BeginPlay();

	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULinearSaveSubsystem* SaveSubsystem = GI->GetSubsystem<ULinearSaveSubsystem>())
		{
			// デフォルト -1 だが、ConsumePendingLoad で、数字が変わっていく
			// 例えば、PlayerController.RestartGame() で スロット 0 が予約
			// -> SaveSubsystem に予約済みのスロット数値が入る
			// -> OpenLevel で再読み込み
			// -> この BeginPlay が走って、ConsumePendingLoad() でチェック
			// -> スロット 0 が予約されているので、SlotToLoad = 0 が入る
			// -> LoadGame(0) として走る、という流れ
			int32 SlotToLoad = -1; 

			if (SaveSubsystem->ConsumePendingLoad(SlotToLoad))
			{
				// 予約があればロード実行（タイトルから、または死亡リトライのケース）
				SaveSubsystem->LoadGame(SlotToLoad);
			}
			else
			{
				// 予約がなければ何もしない（PIEでのデバッグプレイなどのケース）
				UE_LOGFMT(LogTemp, Log, "ALinearGameMode::BeginPlay() No pending load. Start as fresh debug play.");
			}
		}
	}
}

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
