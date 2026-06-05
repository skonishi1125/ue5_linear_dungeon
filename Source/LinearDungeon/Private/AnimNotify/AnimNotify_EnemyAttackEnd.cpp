#include "AnimNotify/AnimNotify_EnemyAttackEnd.h"
#include "Enemies/EnemyBase.h"

void UAnimNotify_EnemyAttackEnd::Notify(
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
			EnemyBase->OnAttackEnd();
		}
	}
}
