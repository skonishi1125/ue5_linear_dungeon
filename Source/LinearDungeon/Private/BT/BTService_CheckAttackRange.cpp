#include "BT/BTService_CheckAttackRange.h"
#include "Logging/StructuredLog.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Enemies/EnemyBase.h"

UBTService_CheckAttackRange::UBTService_CheckAttackRange()
{
	NodeName = TEXT("Check Attack Range");

	// Tick の更新間隔 暫定で .2f としておく
	Interval = 0.2f;
	RandomDeviation = 0.05f; // 負荷分散のためのランダムなブレ
}

void UBTService_CheckAttackRange::TickNode(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds
)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr) return;

	// Blackboard からターゲットを取得
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(FName("CombatTarget")));
	if (TargetActor == nullptr)
	{
		BlackboardComp->SetValueAsBool(GetSelectedBlackboardKey(), false);
		return;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr) return;

	AEnemyBase* EnemyPawn = Cast<AEnemyBase>(AIController->GetPawn());
	if (EnemyPawn == nullptr) return;

	// 距離を計算し、攻撃範囲内かどうかを判定
	const double DistanceToTarget = (TargetActor->GetActorLocation() - EnemyPawn->GetActorLocation()).Size2D();
	const bool bInAttackRange = DistanceToTarget <= EnemyPawn->GetAttackRadius();

	UE_LOGFMT(LogTemp, Log, 
		"Service Tick - Distance: {0}, AttackRadius: {1}, Result: {2}", DistanceToTarget, EnemyPawn->GetAttackRadius(), bInAttackRange
	);

	// 結果を Blackboard に書き込む (InAttackRange に紐付ける)
	BlackboardComp->SetValueAsBool(GetSelectedBlackboardKey(), bInAttackRange);
}
