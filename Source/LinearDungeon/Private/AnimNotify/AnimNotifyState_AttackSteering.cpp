#include "AnimNotify/AnimNotifyState_AttackSteering.h"
#include "Characters/LinearPlayerCharacter.h"

void UAnimNotifyState_AttackSteering::NotifyBegin(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		ALinearPlayerCharacter* LPCharacter = Cast<ALinearPlayerCharacter>(MeshComp->GetOwner());
		if (LPCharacter)
		{
			LPCharacter->OnAttackSteering(true);
		}
	}
}

void UAnimNotifyState_AttackSteering::NotifyEnd(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		ALinearPlayerCharacter* LPCharacter = Cast<ALinearPlayerCharacter>(MeshComp->GetOwner());
		if (LPCharacter)
		{
			LPCharacter->OnAttackSteering(false);
		}
	}
}
