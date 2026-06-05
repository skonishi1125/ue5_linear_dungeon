#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_EnemyAttackEnd.generated.h"

/**
 * Enemy óp AttackEnd ÉgÉäÉKÅ[
 */
UCLASS()
class LINEARDUNGEON_API UAnimNotify_EnemyAttackEnd : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;
	
};
