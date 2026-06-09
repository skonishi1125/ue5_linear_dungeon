#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_WAttackCollision.generated.h"

/**
 * Weapon 用 攻撃判定有効化 - 無効化の Anim Notify State
 * 有効化以外にも、攻撃倍率を Notify 側で決定し、割り当てて Weapon に反映させる役割も持つ
 */
UCLASS()
class LINEARDUNGEON_API UAnimNotifyState_WAttackCollision : public UAnimNotifyState
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
	UPROPERTY(EditAnywhere, Category = "Combat")
	float DamageMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, Category = "Combat")
	float PoiseMultiplier = 1.0f;

	
};
