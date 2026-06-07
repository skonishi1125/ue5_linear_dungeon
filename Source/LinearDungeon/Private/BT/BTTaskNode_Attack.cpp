#include "BT/BTTaskNode_Attack.h"
#include "AIController.h"
#include "Enemies/EnemyBase.h"

UBTTaskNode_Attack::UBTTaskNode_Attack()
{
	NodeName = TEXT("Attack");
}

EBTNodeResult::Type UBTTaskNode_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr) return EBTNodeResult::Failed;

	AEnemyBase* EnemyPawn = Cast<AEnemyBase>(AIController->GetPawn());
	if (EnemyPawn == nullptr) return EBTNodeResult::Failed;

	// EnemyBase 側で公開した攻撃関数を呼び出す
	EnemyPawn->OnPerformAttack();

	// TODO: 設計補足
	// 本来、攻撃アニメーションの終了を待つ場合はここで EBTNodeResult::InProgress を返し、
	// アニメーション終了のデリゲートを受け取ってから FinishLatentTask() で完了を通知するのがベストプラクティス
	// 今回は EnemyBase 側の OnAttackEnd() で状態が管理されているため即座に Succeeded を返して BT を進行させる
	return EBTNodeResult::Succeeded;

}
