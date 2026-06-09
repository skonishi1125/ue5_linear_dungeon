#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_ClearCombatTarget.generated.h"

/**
 * Character を倒した後、CombatTarget をリセットさせるためのノード
 */
UCLASS()
class LINEARDUNGEON_API UBTTaskNode_ClearCombatTarget : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTaskNode_ClearCombatTarget();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector BlackboardKey;

};
