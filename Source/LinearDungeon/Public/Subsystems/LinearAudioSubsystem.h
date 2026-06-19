#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LinearAudioSubsystem.generated.h"

class USoundBase;
class UAudioCOmponent;

// BGM を管理する SUbsystem
UCLASS()
class LINEARDUNGEON_API ULinearAudioSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
public:
	FORCEINLINE float GetCurrentBGMVolume() { return CurrentBGMVolume; }

	// BGMの再生
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayBGM(USoundBase* InBGM);

	// BGMの停止
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void StopBGM();

	// 音量の変更処理
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void SetBGMVolume(float ClampedVolume);

private:
	// 再生中のBGMを保持するコンポーネント
	UPROPERTY()
	TObjectPtr<UAudioComponent> BGMAudioComponent;

	float CurrentBGMVolume = 1.0f;
	
};
