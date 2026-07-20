#include "AnimNotify/AnimNotifyState_ParryWindow.h"
#include "Characters/LinearPlayerCharacter.h"

void UAnimNotifyState_ParryWindow::NotifyBegin(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	float TotalDuration, const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		ALinearPlayerCharacter* Player = Cast<ALinearPlayerCharacter>(MeshComp->GetOwner());
		if (Player)
		{
			Player->SetParryWindowEnabled(true);
		}
	}

}

void UAnimNotifyState_ParryWindow::NotifyEnd(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	const FAnimNotifyEventReference& EventReference
)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		ALinearPlayerCharacter* Player = Cast<ALinearPlayerCharacter>(MeshComp->GetOwner());
		if (Player)
		{
			Player->SetParryWindowEnabled(false);
		}
	}
}
