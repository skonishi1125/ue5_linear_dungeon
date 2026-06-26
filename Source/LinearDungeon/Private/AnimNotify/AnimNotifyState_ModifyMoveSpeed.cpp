#include "AnimNotify/AnimNotifyState_ModifyMoveSpeed.h"
#include "Logging/StructuredLog.h"

#include "Characters/LinearPlayerCharacter.h"

void UAnimNotifyState_ModifyMoveSpeed::NotifyBegin(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	float TotalDuration, const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		if (ALinearPlayerCharacter* LinearPlayerCharacter = Cast<ALinearPlayerCharacter>(MeshComp->GetOwner()))
		{
			LinearPlayerCharacter->OnUsePotionSteering(true);
		}
	}
}

void UAnimNotifyState_ModifyMoveSpeed::NotifyEnd(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		if (ALinearPlayerCharacter* LinearPlayerCharacter = Cast<ALinearPlayerCharacter>(MeshComp->GetOwner()))
		{
			//UE_LOGFMT(LogTemp, Warning, "modifyspeed Notifyend");
			LinearPlayerCharacter->OnUsePotionSteering(false);
		}
	}
}
