#include "BT/BTTask_FindPatrolLocation.h"
#include "Logging/StructuredLog.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Enemies/EnemyBase.h"

UBTTask_FindPatrolLocation::UBTTask_FindPatrolLocation()
{
	NodeName = TEXT("Find Patrol Location");
}

EBTNodeResult::Type UBTTask_FindPatrolLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	// AI Controller を取得し、その Possess 先から EnemyBase を取得できる
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AEnemyBase* EnemyBase = Cast<AEnemyBase>(AIController->GetPawn());
	if (EnemyBase == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// EnemyBase で用意した PatrolTarget 取得処理から次の巡回先 Actor を取得し、BB に位置情報 Vector 書込
	AActor* NextTarget = EnemyBase->OnGetNextPatrolTarget();
	if (NextTarget != nullptr)
	{
		UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsVector(
				GetSelectedBlackboardKey(), NextTarget->GetActorLocation()
			);
			return EBTNodeResult::Succeeded;
		}
	}

	return EBTNodeResult::Failed;
}
