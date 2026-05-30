#include "AnimNotify/AnimNotifyState_RollingField.h"
#include "Characters/LinearPlayerCharacter.h"

// Notify 区間進入時の処理
void UAnimNotifyState_RollingField::NotifyBegin(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	float TotalDuration, const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp)
	{
		// アニメを再生しているのは、SkeletalMeshComp
		// USkeletalMeshComponent* MeshComp から、LinearPlayerCharacter を取得
		ALinearPlayerCharacter* LPCharacter = Cast<ALinearPlayerCharacter>(MeshComp->GetOwner());
		if (LPCharacter)
		{
			LPCharacter->OnRollingFieldNotifyBegin();
		}
	}
}

// Notify 終了時の処理
void UAnimNotifyState_RollingField::NotifyEnd(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (MeshComp)
	{
		ALinearPlayerCharacter* LPCharacter = Cast<ALinearPlayerCharacter>(MeshComp->GetOwner());
		if (LPCharacter)
		{
			// キャラクター側で定義した破棄関数を呼ぶ
			LPCharacter->OnRollingFieldNotifyEnd();
		}
	}
}
