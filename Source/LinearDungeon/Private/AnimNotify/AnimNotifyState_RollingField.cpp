#include "AnimNotify/AnimNotifyState_RollingField.h"

// Notify 区間進入時の処理
void UAnimNotifyState_RollingField::NotifyBegin(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	float TotalDuration, const FAnimNotifyEventReference& EventReference
)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp && FieldActorClass)
	{
		// アニメを再生しているのは、SkeletalMeshComp
		// USkeletalMeshComponent* MeshComp から、LinearPlayerCharacter を取得
		AActor* OwnerActor = MeshComp->GetOwner();
		if (OwnerActor)
		{
			// Actor を Spawn させるため、World 取得
			// (Spawn させる Actor は、FieldSystem を持つ一時的な Actor)
			UWorld* World = OwnerActor->GetWorld();
			if (World)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = OwnerActor; // 生み出す FieldSystem の親を LPCharacter に設定
				SpawnedFieldActor = World->SpawnActor<AActor>(
					// 親と同じ Locate, Rotate などでスポーン
					FieldActorClass, OwnerActor->GetActorLocation(), OwnerActor->GetActorRotation(), SpawnParams
				);
				if (SpawnedFieldActor)
				{
					// LPCharacter にアタッチ
					SpawnedFieldActor->AttachToComponent(
						OwnerActor->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale
					);
				}
			}
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
	if (SpawnedFieldActor)
	{
		// 作成した FieldSystem Actor を破棄
		SpawnedFieldActor->Destroy();
		SpawnedFieldActor = nullptr;
	}
}
