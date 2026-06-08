#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EnemyHitReactEnd.generated.h"

/**
 * Enemy 用 HitReactEnd トリガー（BT の怯みなどの操作用）
 */
UCLASS()
class LINEARDUNGEON_API UAnimNotify_EnemyHitReactEnd : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;
	
};
