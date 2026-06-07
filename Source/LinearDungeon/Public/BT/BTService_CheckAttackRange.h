#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"

#include "BTService_CheckAttackRange.generated.h"

UCLASS()
class LINEARDUNGEON_API UBTService_CheckAttackRange : public UBTService_BlackboardBase
{
	GENERATED_BODY()
public:
	UBTService_CheckAttackRange();

protected:
	// Service がアクティブな間、指定されたインターバルで実行される関数
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
