#include "BT/BTService_CheckTargetReach.h"
#include "Logging/StructuredLog.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/Actor.h"


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

	// CombatTarget の 座標が、NavMesh 上に存在するかの判定結果を IsTargetReachableKey に格納
	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (!NavSys)
	{
		BlackboardComp->SetValueAsBool(IsTargetReachableKey.SelectedKeyName, false);
		return;
	}

	FNavLocation ProjectedLocation;
	const bool bIsOnNavMesh = NavSys->ProjectPointToNavigation(
		TargetActor->GetActorLocation(),
		ProjectedLocation,
		QueryExtent // 許容範囲(Location から、(100, 100, 100) 程度の誤差なら OK
	);

	BlackboardComp->SetValueAsBool(IsTargetReachableKey.SelectedKeyName, bIsOnNavMesh);

}
