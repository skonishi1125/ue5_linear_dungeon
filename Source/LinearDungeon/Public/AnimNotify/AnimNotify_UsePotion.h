#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_UsePotion.generated.h"

/**
 * 指定したタイミングで、対象の体力を回復する
 */
UCLASS()
class LINEARDUNGEON_API UAnimNotify_UsePotion : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;

	
};
