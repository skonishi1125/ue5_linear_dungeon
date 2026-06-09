#include "AnimNotify/AnimNotifyState_WAttackCollision.h"
#include "Characters/LinearPlayerCharacter.h"

void UAnimNotifyState_WAttackCollision::NotifyBegin(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	float TotalDuration, const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp)
	{
		ALinearPlayerCharacter* LPCharacter = Cast<ALinearPlayerCharacter>(MeshComp->GetOwner());
		if (LPCharacter)
		{
			LPCharacter->OnWeaponCollisionEnabled(
				ECollisionEnabled::QueryOnly, DamageMultiplier, PoiseMultiplier
			);
		}
	}

}

void UAnimNotifyState_WAttackCollision::NotifyEnd(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (MeshComp)
	{
		ALinearPlayerCharacter* LPCharacter = Cast<ALinearPlayerCharacter>(MeshComp->GetOwner());
		if (LPCharacter)
		{
			LPCharacter->OnWeaponCollisionDisabled(ECollisionEnabled::NoCollision);
		}
	}

}
