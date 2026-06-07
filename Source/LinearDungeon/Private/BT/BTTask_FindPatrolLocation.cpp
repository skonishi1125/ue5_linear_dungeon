#include "BT/BTTask_FindPatrolLocation.h"
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

	// AI Controller Ç∆ Pawn ÇÃéÊìæ
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AEnemyBase* EnemyPawn = Cast<AEnemyBase>(AIController->GetPawn());
	if (EnemyPawn == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	// EnemyBase Ç©ÇÁéüÇÃèÑâÒêÊ Actor ÇéÊìæ
	AActor* NextTarget = EnemyPawn->GetNextPatrolTarget();
	if (NextTarget != nullptr)
	{
		// Blackboard Ç…à íuèÓïÒÇ Vector Ç∆ÇµÇƒèëÇ´çûÇﬁ
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), NextTarget->GetActorLocation());

		// ê¨å˜Ç∆ÇµÇƒ Task ÇèIóπ
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}
