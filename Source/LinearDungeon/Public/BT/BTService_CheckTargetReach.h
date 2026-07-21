#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlackboardBase.h"

#include "BTService_CheckTargetReach.generated.h"

UCLASS()
class LINEARDUNGEON_API UBTService_CheckTargetReach : public UBTService_BlackboardBase
{
	GENERATED_BODY()

public:
	UBTService_CheckTargetReach();
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
private:
	// BlackBoard ë§Ç≈çÏê¨ÇµÇΩÅAIsTargetReachable Ç BT ê›íËéûÇ…äÑÇËìñÇƒÇÈÇ±Ç∆
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector IsTargetReachableKey;

	UPROPERTY(EditAnywhere, Category = "Navigation")
	FVector QueryExtent = FVector(100.f, 100.f, 250.f);

};
