#include "Subsystems/LinearAudioSubsystem.h"
#include "Logging/StructuredLog.h"

#include "Sound/SoundBase.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

void ULinearAudioSubsystem::PlayBGM(USoundBase* InBGM, EBGMType BGMType)
{
	if (!InBGM) return;
	if (BGMAudioComponent && BGMAudioComponent->GetSound() == InBGM) return;

	// 再生中の BGM を停止して、再生処理を行う
	StopBGM();

	UE_LOGFMT(LogTemp, Log, "ULinearAudioSubsystem::PlayBGM");


	BGMAudioComponent = UGameplayStatics::CreateSound2D(GetWorld(), InBGM);
	if (BGMAudioComponent)
	{
		BGMAudioComponent->Play();
		// 別の BGM を再生するときも、今の音量を保持する
		BGMAudioComponent->SetVolumeMultiplier(CurrentBGMVolume);
	}

}

void ULinearAudioSubsystem::StopBGM()
{
	if (BGMAudioComponent && BGMAudioComponent->IsPlaying())
	{
		BGMAudioComponent->Stop();
	}
}

void ULinearAudioSubsystem::SetBGMVolume(float ClampedVolume)
{
	// Volume は 0.0001 ~ 1.0 の範囲を想定
	// 0 を渡すと、UE 側がサウンドを停止させるような挙動になる
	// （BGMSlider 側でも Clamp している）
	// また、MetaSound 側の Detail > Virtualization Mode で、Play when Silent としておく
	// これで、BGM が 0 になっても裏で流す処理が行われ続ける

	// 流れている BGM の MetaSound に対して行うのではなく、
	// MetaSound を流している大元が BGMAudioComponent なので、そちらに対して設定する形になる。
	if (BGMAudioComponent && BGMAudioComponent->IsPlaying())
	{
		BGMAudioComponent->SetVolumeMultiplier(ClampedVolume);
		CurrentBGMVolume = ClampedVolume;
		UE_LOGFMT(LogTemp, Warning, "ULinearAudioSubsystem::SetBGMVolume() Settings ClampedVolume!");
	}

}
