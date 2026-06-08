#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_CheckCombo.generated.h"

/**
 * 分岐判定を行うための、コンボ用 Anim Notify
 */
UCLASS()
class LINEARDUNGEON_API UAnimNotifyState_CheckCombo : public UAnimNotifyState
{
	GENERATED_BODY()
	virtual void Notify(
		USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;
	
};
