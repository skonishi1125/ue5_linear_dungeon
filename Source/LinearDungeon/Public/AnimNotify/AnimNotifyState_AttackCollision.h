#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Characters/CharacterTypes.h"

#include "AnimNotifyState_AttackCollision.generated.h"

/*
* 敵の攻撃判定を有効化する区間を示す NotifyState
* ※Character には適用していない
*/
UCLASS()
class LINEARDUNGEON_API UAnimNotifyState_AttackCollision : public UAnimNotifyState
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

	// エディタ上で、どの攻撃なのか判定するための Enum
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	EAttackCollisionType CollisionType = EAttackCollisionType::EAC_RightHand;

private:
	UPROPERTY(EditAnywhere, Category = "Combat")
	float DamageMultiplier = 1.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PoiseMultiplier = 1.0f;
	
};
