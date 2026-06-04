#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_CheckCombo.generated.h"

/**
 * コンボ攻撃 分岐判定用のトリガー
 */
UCLASS()
class LINEARDUNGEON_API UAnimNotify_CheckCombo : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(
		USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference
	) override;
	
};
