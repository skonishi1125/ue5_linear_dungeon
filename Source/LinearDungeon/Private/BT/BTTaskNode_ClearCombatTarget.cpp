#include "BT/BTTaskNode_ClearCombatTarget.h"
#include "Logging/StructuredLog.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTaskNode_ClearCombatTarget::UBTTaskNode_ClearCombatTarget()
{
	NodeName = TEXT("ClearCombatTarget");
}

EBTNodeResult::Type UBTTaskNode_ClearCombatTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	//UE_LOGFMT(LogTemp, Warning, "UBTTaskNode_ClearCombatTarget::ExecuteTask");

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = AIController->GetBlackboardComponent();
	if (BB == nullptr) return EBTNodeResult::Failed;

	BB->ClearValue(BlackboardKey.SelectedKeyName);
	//BB->SetValueAsObject(FName("CombatTarget"), nullptr);

	return EBTNodeResult::Succeeded;
}
