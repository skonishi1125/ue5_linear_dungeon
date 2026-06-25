#include "AnimNotifyState_AdjustIKAlpha.h"
#include "Characters/LinearPlayerAnimInstance.h"
#include "Components/SkeletalMeshComponent.h"

void UAnimNotifyState_AdjustIKAlpha::NotifyBegin(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	float TotalDuration, const FAnimNotifyEventReference& EventReference
)
{
	if (!MeshComp)
	{
		return;
	}

	ULinearPlayerAnimInstance* AnimInstance =
		Cast<ULinearPlayerAnimInstance>(MeshComp->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->SetLeftHandIKAlphaMultiplierTarget(IKAlphaMultiplier);
	}

}

void UAnimNotifyState_AdjustIKAlpha::NotifyEnd(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
)
{
	// Alpha ‚ð Œ³‚Ì’l(1.0)‚É–ß‚·
	if (!MeshComp)
	{
		return;
	}
	ULinearPlayerAnimInstance* AnimInstance =
		Cast<ULinearPlayerAnimInstance>(MeshComp->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->SetLeftHandIKAlphaMultiplierTarget(1.f);
	}
}
