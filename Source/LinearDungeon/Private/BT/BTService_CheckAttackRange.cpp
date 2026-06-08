#include "BT/BTService_CheckAttackRange.h"
#include "Logging/StructuredLog.h"
#include "LinearDungeon/DebugMacros.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Enemies/EnemyBase.h"

UBTService_CheckAttackRange::UBTService_CheckAttackRange()
{
	NodeName = TEXT("Check Attack Range");

	Interval = 0.2f; // Tick の更新間隔 .2f などで緩めにしておくと、負荷分散につながる
	RandomDeviation = 0.05f; // 負荷分散のためのランダムなブレ

	// Service が Active になった時の通知フラグ
	// これで OnBecomeRelevant が走る
	bNotifyBecomeRelevant = true;
}

void UBTService_CheckAttackRange::OnBecomeRelevant(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory
)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	DrawDebugDistanceToTarget(OwnerComp);
	//UE_LOGFMT(LogTemp, Log, "UBTService_CheckAttackRange::OnBecomeRelevant()");

}

void UBTService_CheckAttackRange::TickNode(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds
)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr) return;

	// Blackboard から TargetActor 取得
	// ※ BB 側の TargetActor 自体は、ALinearEnemyAIController::OnTargetDetected の視覚処理で取得している
	// AI Controller には BT を割り当てているので、そこ経由で BB を参照できている
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(BlackboardTargetName));
	if (TargetActor == nullptr)
	{
		BlackboardComp->SetValueAsBool(GetSelectedBlackboardKey(), false);
		return;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr) return;

	AEnemyBase* EnemyBase = Cast<AEnemyBase>(AIController->GetPawn());
	if (EnemyBase == nullptr) return;

	// 距離を計算し、攻撃範囲内かどうかを判定
	const double DistanceToTarget = (TargetActor->GetActorLocation() - EnemyBase->GetActorLocation()).Size2D();
	const bool bInAttackRange = DistanceToTarget <= EnemyBase->OnGetAttackRadius();

	//UE_LOGFMT(LogTemp, Log, 
	//	"Service Tick - Distance: {0}, AttackRadius: {1}, Result: {2}", DistanceToTarget, EnemyBase->OnGetAttackRadius(), bInAttackRange
	//);

	// 結果を Blackboard に書き込む (InAttackRange に紐付ける)
	BlackboardComp->SetValueAsBool(GetSelectedBlackboardKey(), bInAttackRange);
}

void UBTService_CheckAttackRange::DrawDebugDistanceToTarget(UBehaviorTreeComponent& OwnerComp)
{
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (BlackboardComp == nullptr) return;

	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(BlackboardTargetName));
	if (TargetActor == nullptr)
	{
		BlackboardComp->SetValueAsBool(GetSelectedBlackboardKey(), false);
		return;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	AEnemyBase* EnemyBase = Cast<AEnemyBase>(AIController->GetPawn());

	if (AIController && EnemyBase)
	{
		DRAW_SPHERE(TargetActor->GetActorLocation());
		DRAW_SPHERE(EnemyBase->GetActorLocation());
	}
}
