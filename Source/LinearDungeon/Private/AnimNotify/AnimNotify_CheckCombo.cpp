#include "AnimNotify/AnimNotify_CheckCombo.h"
#include "Characters/LinearPlayerCharacter.h"

void UAnimNotify_CheckCombo::Notify(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		ALinearPlayerCharacter* LPCharacter = Cast<ALinearPlayerCharacter>(MeshComp->GetOwner());
		if (LPCharacter)
		{
			LPCharacter->OnCheckCombo();
		}
	}
}
