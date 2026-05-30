#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_RollingField.generated.h"

UCLASS()
class LINEARDUNGEON_API UAnimNotifyState_RollingField : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	// AM 側で、どういった型に対して設定するか
	// TODO: AM から BP_LPCharacter がどのように取得されているのかを調べてみる
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Field")
	TSubclassOf<AActor> FieldActorClass;

	virtual void NotifyBegin(
		USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
		float TotalDuration, const FAnimNotifyEventReference& EventReference
	) override;

	virtual void NotifyEnd(
		USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
		const FAnimNotifyEventReference& EventReference
	) override;

private:
	// FieldSystem を持つ Actor を割り当てる予定のポインタ
	UPROPERTY()
	TObjectPtr<AActor> SpawnedFieldActor;
	
};
