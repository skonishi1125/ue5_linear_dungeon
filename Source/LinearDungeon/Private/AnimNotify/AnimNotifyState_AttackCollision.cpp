#include "AnimNotify/AnimNotifyState_AttackCollision.h"
#include "Enemies/EnemyBase.h"

void UAnimNotifyState_AttackCollision::NotifyBegin(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		AEnemyBase* EnemyBase = Cast<AEnemyBase>(MeshComp->GetOwner());
		if (EnemyBase)
		{
			EnemyBase->OnAttackCollisionNotifyBegin(
				CollisionType, DamageMultiplier, PoiseMultiplier
			);
		}
	}

}

void UAnimNotifyState_AttackCollision::NotifyEnd(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		AEnemyBase* EnemyBase = Cast<AEnemyBase>(MeshComp->GetOwner());
		if (EnemyBase)
		{
			EnemyBase->OnAttackCollisionNotifyEnd();
		}
	}

}
