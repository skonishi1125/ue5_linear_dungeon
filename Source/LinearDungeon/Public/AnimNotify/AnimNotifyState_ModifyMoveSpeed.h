#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_ModifyMoveSpeed.generated.h"

/**
 * 区間中、Character の 移動速度を調整する
 * 例えばアイテム使用中などに速度を落としたりする。
 */
UCLASS()
class LINEARDUNGEON_API UAnimNotifyState_ModifyMoveSpeed : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		float TotalDuration, const FAnimNotifyEventReference& EventReference
	) override;

	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;

};
