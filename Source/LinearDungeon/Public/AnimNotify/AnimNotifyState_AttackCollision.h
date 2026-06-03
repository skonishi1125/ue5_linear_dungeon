// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Characters/CharacterTypes.h"

#include "AnimNotifyState_AttackCollision.generated.h"


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
	
};
