#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_Attack.generated.h"

UCLASS()
class LINEARDUNGEON_API UBTTaskNode_Attack : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_Attack();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
