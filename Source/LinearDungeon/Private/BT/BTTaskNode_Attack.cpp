#include "BT/BTTaskNode_Attack.h"
#include "AIController.h"
#include "Enemies/EnemyBase.h"

UBTTaskNode_Attack::UBTTaskNode_Attack()
{
	NodeName = TEXT("Attack");

	// Task を実行する AI ごとに、専用インスタンスを用意するようにする
	// （CachedOwnerComp = &OwnerComp; など、各 Task ごとにメンバ変数を弄る設計とする）
	bCreateNodeInstance = true;

}

EBTNodeResult::Type UBTTaskNode_Attack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr) return EBTNodeResult::Failed;

	AEnemyBase* EnemyPawn = Cast<AEnemyBase>(AIController->GetPawn());
	if (EnemyPawn == nullptr) return EBTNodeResult::Failed;

	// 攻撃処理
	CachedOwnerComp = &OwnerComp; // AttackEnd で OwnerComp を使うために格納しておく
	EnemyPawn->OnAttackEndDelegate.AddUniqueDynamic(this, &UBTTaskNode_Attack::HandleAttackFinished);
	EnemyPawn->OnPerformAttack();

	return EBTNodeResult::InProgress;

}

void UBTTaskNode_Attack::HandleAttackFinished()
{
	if (CachedOwnerComp)
	{
		AAIController* AIController = CachedOwnerComp->GetAIOwner();
		if (AEnemyBase* EnemyPawn = Cast<AEnemyBase>(AIController->GetPawn()))
		{
			// 多重実行を防ぐために解除しておく
			EnemyPawn->OnAttackEndDelegate.RemoveDynamic(this, &UBTTaskNode_Attack::HandleAttackFinished);
		}

		// Task の成功を BT に通知
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
}
