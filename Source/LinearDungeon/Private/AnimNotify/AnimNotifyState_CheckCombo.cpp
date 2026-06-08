#include "AnimNotify/AnimNotifyState_CheckCombo.h"
#include "Characters/LinearPlayerCharacter.h"

void UAnimNotifyState_CheckCombo::Notify(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	const FAnimNotifyEventReference& EventReference
)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		ALinearPlayerCharacter* Player = Cast<ALinearPlayerCharacter>(MeshComp->GetOwner());
		if (Player)
		{
			Player->CheckCombo();
		}
	}
}
