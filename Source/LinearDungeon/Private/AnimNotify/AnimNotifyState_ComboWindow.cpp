#include "AnimNotify/AnimNotifyState_ComboWindow.h"
#include "Characters/LinearPlayerCharacter.h"

void UAnimNotifyState_ComboWindow::NotifyBegin(
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
			// 先行入力受付期間であることを示すフラグを有効化
			LPCharacter->OnCanSaveAttack(true);
		}
	}
}

void UAnimNotifyState_ComboWindow::NotifyEnd(
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
			LPCharacter->OnCanSaveAttack(false);
		}
	}
}


