#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateEnemyAIState.generated.h"

UCLASS()
class LINEARDUNGEON_API UBTService_UpdateEnemyAIState : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_UpdateEnemyAIState();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	// 対象となるターゲット（ = CombatTarget）
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector CombatTargetKey;

	// 更新対象となるState（ = EEnemyAIState）
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector StateKey;

};
