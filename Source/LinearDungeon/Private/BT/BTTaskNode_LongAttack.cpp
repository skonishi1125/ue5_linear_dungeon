#include "BT/BTTaskNode_LongAttack.h"
#include "Logging/StructuredLog.h"

#include "Controllers/LinearEnemyAIController.h"
#include "Enemies/EnemyBase.h"
#include "BT/BTService_CheckAttackRange.h"
#include "BehaviorTree/BlackboardComponent.h"


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

	// 攻撃処理
	CachedOwnerComp = &OwnerComp; // AttackEnd で OwnerComp を使うために格納しておく
	EnemyPawn->OnAttackEndDelegate.AddUniqueDynamic(this, &UBTTaskNode_LongAttack::HandleLongAttackFinished);
	EnemyPawn->OnPerformLongAttack();

	return EBTNodeResult::InProgress;

}

EBTNodeResult::Type UBTTaskNode_LongAttack::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// Staggered 等の割り込みが発生し、強制中断された場合の処理
	if (AAIController* AIController = OwnerComp.GetAIOwner())
	{
		if (AEnemyBase* EnemyPawn = Cast<AEnemyBase>(AIController->GetPawn()))
		{
			//デリゲート解除
			EnemyPawn->OnAttackEndDelegate.RemoveDynamic(this, &UBTTaskNode_LongAttack::HandleLongAttackFinished);
		}
	}
	CachedOwnerComp = nullptr;

	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTaskNode_LongAttack::HandleLongAttackFinished()
{
	if (!IsValid(CachedOwnerComp)) return;

	AAIController* AIController = CachedOwnerComp->GetAIOwner();
	if (!IsValid(AIController)) return;

	if (AEnemyBase* EnemyBase = Cast<AEnemyBase>(AIController->GetPawn()))
	{
		EnemyBase->OnAttackEndDelegate.RemoveDynamic(this, &UBTTaskNode_LongAttack::HandleLongAttackFinished);
	}

	if (ALinearEnemyAIController* LinearAIController = Cast<ALinearEnemyAIController>(AIController))
	{
		if (UBlackboardComponent* BB = CachedOwnerComp->GetBlackboardComponent())
		{
			// 現在の State を Blackboard から取得し、Attacking の場合だけ Chase に移行する
			// ※このガードが無いと、パリィや被弾で攻撃が中断されて Staggered になったとき、
			// 即座に Staggered -> Chase に戻って、素早く攻撃が繰り返されてしまう
			uint8 CurrentStateNum = BB->GetValueAsEnum(FName("CurrentEnemyAIState"));
			EEnemyAIState CurrentState = static_cast<EEnemyAIState>(CurrentStateNum);

			if (CurrentState == EEnemyAIState::EEAIS_Attacking)
			{
				LinearAIController->ChangeAIState(EEnemyAIState::EEAIS_Chase);
			}
		}
	}

	// Task の成功を BT に通知
	FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);

}
