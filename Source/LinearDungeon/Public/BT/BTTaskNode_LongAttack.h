#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_LongAttack.generated.h"

UCLASS()
class LINEARDUNGEON_API UBTTaskNode_LongAttack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_LongAttack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UFUNCTION()
	void HandleLongAttackFinished();

private:
	TObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;

};
