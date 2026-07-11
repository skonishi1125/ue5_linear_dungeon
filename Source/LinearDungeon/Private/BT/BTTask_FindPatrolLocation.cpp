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

	// AI Controller ‚ğæ“¾‚µA‚»‚Ì Possess æ‚©‚ç EnemyBase ‚ğæ“¾‚Å‚«‚é
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

	// EnemyBase ‚©‚çŸ‚Ì„‰ñæ Actor ‚ğæ“¾
	// Blackboard ‚ÉˆÊ’uî•ñ‚ğ Vector ‚Æ‚µ‚Ä‘‚«‚Ş
	AActor* NextTarget = EnemyBase->OnGetNextPatrolTarget();
	if (NextTarget != nullptr)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsVector(
			GetSelectedBlackboardKey(), NextTarget->GetActorLocation()
		);

		return EBTNodeResult::Succeeded; // ¬Œ÷
	}

	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
	{
		UObject* CT = BB->GetValueAsObject(FName("CombatTarget"));
	}

	return EBTNodeResult::Failed;
}
