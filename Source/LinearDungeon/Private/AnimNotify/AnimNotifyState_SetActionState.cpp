#include "AnimNotify/AnimNotifyState_SetActionState.h"
#include "Characters/LinearPlayerCharacter.h"

void UAnimNotifyState_SetActionState::NotifyBegin(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	float TotalDuration, const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		if (ALinearPlayerCharacter* LinearPlayerCharacter = Cast<ALinearPlayerCharacter>(MeshComp->GetOwner()))
		{
			LinearPlayerCharacter->SetCharacterActionState(ActionState);
		}
	}
}

void UAnimNotifyState_SetActionState::NotifyEnd(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		if (ALinearPlayerCharacter* LinearPlayerCharacter = Cast<ALinearPlayerCharacter>(MeshComp->GetOwner()))
		{
			LinearPlayerCharacter->SetCharacterActionState(EActionState::EAS_Unoccupied);
		}
	}
}
