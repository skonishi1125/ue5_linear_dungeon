#include "AnimNotify/AnimNotifyState_AdjustAnimScale.h"
#include "GameFramework/Character.h"

void UAnimNotifyState_AdjustAnimScale::NotifyBegin(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

    if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
    {
		Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
    }
}

void UAnimNotifyState_AdjustAnimScale::NotifyEnd(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (ACharacter* Character = Cast<ACharacter>(MeshComp->GetOwner()))
	{
		Character->SetAnimRootMotionTranslationScale(1.0f);
	}
}
