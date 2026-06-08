#include "AnimNotify/AnimNotify_EnemyHitReactEnd.h"
#include "Enemies/EnemyBase.h"

void UAnimNotify_EnemyHitReactEnd::Notify(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	const FAnimNotifyEventReference& EventReference
)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		AEnemyBase* EnemyBase = Cast<AEnemyBase>(MeshComp->GetOwner());
		if (EnemyBase)
		{
			EnemyBase->OnStaggerEnd();
		}
	}

}
