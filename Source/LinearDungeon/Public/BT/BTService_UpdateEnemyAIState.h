#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateEnemyAIState.generated.h"

// Enemy BT Ç…Ç®ÇØÇÈÅAAI State ÇåàíËÇ∑ÇÈç™åπÇ∆Ç»ÇÈèàóù
UCLASS()
class LINEARDUNGEON_API UBTService_UpdateEnemyAIState : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_UpdateEnemyAIState();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CombatTargetKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector AIStateKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CombatRangeStateKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector HasLineOfSightKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsTargetReachableKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsTargetDiedKey;

};
