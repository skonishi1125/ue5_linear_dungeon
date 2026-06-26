#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Characters/CharacterTypes.h"

#include "AnimNotifyState_SetActionState.generated.h"

/**
 * 指定した区間の開始 / 終了時に State を切り替える
 * 現状 UsePotion でしか使っていないが、汎用的な処理にはしている
 */
UCLASS()
class LINEARDUNGEON_API UAnimNotifyState_SetActionState : public UAnimNotifyState
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
	UPROPERTY(EditAnywhere, Category = "Action State", meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_UsingItem;
	
};
