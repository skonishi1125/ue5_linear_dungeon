#include "BT/BTTaskNode_LongAttack.h"
#include "Logging/StructuredLog.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemies/EnemyBase.h"
#include "BT/BTService_CheckAttackRange.h"

UBTTaskNode_LongAttack::UBTTaskNode_LongAttack()
{
	NodeName = TEXT("LongAttack");

	// Task を実行する AI ごとに、専用インスタンスを用意するようにする
	// （CachedOwnerComp = &OwnerComp; など、各 Task ごとにメンバ変数を弄る設計とする）
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTaskNode_LongAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	//UE_LOGFMT(LogTemp, Warning, "[LongAttack] Execute enter");


	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController == nullptr) return EBTNodeResult::Failed;

	AEnemyBase* EnemyPawn = Cast<AEnemyBase>(AIController->GetPawn());
	if (EnemyPawn == nullptr) return EBTNodeResult::Failed;

	if (UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
	{
		BlackboardComp->SetValueAsBool(FName("IsAttacking"), true);
	}

	// 攻撃処理
	CachedOwnerComp = &OwnerComp; // AttackEnd で OwnerComp を使うために格納しておく
	EnemyPawn->OnAttackEndDelegate.AddUniqueDynamic(this, &UBTTaskNode_LongAttack::HandleLongAttackFinished);
	EnemyPawn->OnPerformLongAttack();

	return EBTNodeResult::InProgress;

}

void UBTTaskNode_LongAttack::HandleLongAttackFinished()
{
	if (CachedOwnerComp)
	{
		if (UBlackboardComponent* BlackboardComp = CachedOwnerComp->GetBlackboardComponent())
		{
			BlackboardComp->SetValueAsBool(FName("IsAttacking"), false);
			BlackboardComp->SetValueAsEnum(FName("CombatRangeState"), static_cast<uint8>(ECombatRangeState::None));
		}

		AAIController* AIController = CachedOwnerComp->GetAIOwner();
		if (AEnemyBase* EnemyPawn = Cast<AEnemyBase>(AIController->GetPawn()))
		{
			// 多重実行を防ぐために解除しておく
			EnemyPawn->OnAttackEndDelegate.RemoveDynamic(this, &UBTTaskNode_LongAttack::HandleLongAttackFinished);
		}

		// Task の成功を BT に通知
		FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
	}
}


