#include "AnimNotify/AnimNotifyState_TrackingTarget.h"
#include "Enemies/EnemyBase.h"

void UAnimNotifyState_TrackingTarget::NotifyBegin(
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
			EnemyBase->OnTrackingTarget(true);
		}
	}
}

void UAnimNotifyState_TrackingTarget::NotifyEnd(
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
			EnemyBase->OnTrackingTarget(false);
		}
	}

}
