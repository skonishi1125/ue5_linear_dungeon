#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_AdjustIKAlpha.generated.h"

// Ç±ÇÃãÊä‘ÇÃä‘ÅAIK Alpha Çé„ÇﬂÇÈ
UCLASS()
class LINEARDUNGEON_API UAnimNotifyState_AdjustIKAlpha : public UAnimNotifyState
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

private:
	UPROPERTY(EditAnywhere, Category = "IK", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float IKAlphaMultiplier = 1.0f;
	
};
