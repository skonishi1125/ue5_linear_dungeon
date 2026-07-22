#include "BT/BTService_CheckTargetReach.h"
#include "Logging/StructuredLog.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/Actor.h"
#include "AIController.h"


UBTService_CheckTargetReach::UBTService_CheckTargetReach()
{
	NodeName = TEXT("Check Target Reachability");
	Interval = 0.2f;
	RandomDeviation = 0.05f;
}

void UBTService_CheckTargetReach::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (! BlackboardComp) return;

	// BB 側では CombatTarget を指定する想定
	// Actor に Cast できなければ、追いかけられないと判断する
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(GetSelectedBlackboardKey()));
	if (!IsValid(TargetActor))
	{
		BlackboardComp->SetValueAsBool(IsTargetReachableKey.SelectedKeyName, false);
		return;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController || !AIController->GetPawn()) return;

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	bool bIsReachable = false;

	if (NavSys)
	{
		// AIのNavAgentPropertiesと現在座標に適合するNavigationDataを取得
		const ANavigationData* NavData = NavSys->GetNavDataForProps(
			AIController->GetNavAgentPropertiesRef(),
			AIController->GetPawn()->GetActorLocation()
		);

		if (NavData)
		{
			FPathFindingQuery Query(
				AIController,
				*NavData,
				AIController->GetPawn()->GetActorLocation(),
				TargetActor->GetActorLocation()
			);

			// AIからターゲットへの有効な経路が存在するかを判定
			bIsReachable = NavSys->TestPathSync(Query, EPathFindingMode::Regular);
		}
	}

	BlackboardComp->SetValueAsBool(IsTargetReachableKey.SelectedKeyName, bIsReachable);

}
