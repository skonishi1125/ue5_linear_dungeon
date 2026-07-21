#include "BT/BTService_UpdateEnemyAIState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/CharacterTypes.h"

UBTService_UpdateEnemyAIState::UBTService_UpdateEnemyAIState()
{
	NodeName = TEXT("Update Enemy AI State");

	Interval = 0.2f;
	RandomDeviation = 0.05f;
	bCreateNodeInstance = true;
}

void UBTService_UpdateEnemyAIState::TickNode(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds
)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	// TODO: 怯み

	UObject* CombatTarget = BlackboardComp->GetValueAsObject(CombatTargetKey.SelectedKeyName);

	// ターゲットの有無で Patrol / Chase を切り替える
	if (!IsValid(CombatTarget))
	{
		BlackboardComp->SetValueAsEnum(StateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::EEAIS_Patrol));
	}
	else
	{
		BlackboardComp->SetValueAsEnum(StateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::EEAIS_Chase));
	}

}


