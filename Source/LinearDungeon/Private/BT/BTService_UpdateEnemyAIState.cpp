#include "BT/BTService_UpdateEnemyAIState.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/CharacterTypes.h"
#include "AIController.h"
#include "Enemies/EnemyBase.h"
#include "NavigationSystem.h"


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

	uint8 CurrentStateNum = BlackboardComp->GetValueAsEnum(AIStateKey.SelectedKeyName);
	EEnemyAIState CurrentState = static_cast<EEnemyAIState>(CurrentStateNum);

	if (BlackboardComp->GetValueAsBool(IsTargetDiedKey.SelectedKeyName))
	{
		BlackboardComp->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::EEAIS_Victory));
		return; 
	}

	// 現在の State が怯み, 攻撃中は自動更新は行わない
	if (CurrentState == EEnemyAIState::EEAIS_Staggered || CurrentState == EEnemyAIState::EEAIS_Attacking)
	{
		return;
	}

	UObject* CombatTarget = BlackboardComp->GetValueAsObject(CombatTargetKey.SelectedKeyName);
	bool bHasLineOfSight = BlackboardComp->GetValueAsBool(HasLineOfSightKey.SelectedKeyName);

	// ターゲットの有無で Patrol / Chase を切り替える
	if (!IsValid(CombatTarget))
	{
		BlackboardComp->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::EEAIS_Patrol));

		// TODO: PatrolTarget の所持可否を t/f で持つ 暫定で true
		//bool bHasPatrolRoute = true;
		//if (bHasPatrolRoute)
		//{
		//	BlackboardComp->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::EEAIS_Patrol));
		//}
		//else
		//{
		//	BlackboardComp->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::EEAIS_Idle));
		//}
	}
	else
	{
		if (bHasLineOfSight)
		{
			// Chase処理
			// 敵の攻撃範囲を EnemyBase から取得して、距離に応じた State を入れる
			AAIController* AIController = OwnerComp.GetAIOwner();
			if (!AIController || !AIController->GetPawn()) return;

			AEnemyBase* EnemyBase = Cast<AEnemyBase>(AIController->GetPawn());
			if (EnemyBase == nullptr) return;

			AActor* CombatTargetActor = Cast<AActor>(CombatTarget);

			if (CombatTargetActor)
			{
				// 到達可能性 (Reachability) の判定
				// 例えば Chase 中に、Player が NavMesh の外に出た時などの処理を決める
				bool bIsReachable = BlackboardComp->GetValueAsBool(IsTargetReachableKey.SelectedKeyName);

				if (! bIsReachable)
				{
					BlackboardComp->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::EEAIS_Patrol));
					BlackboardComp->SetValueAsEnum(CombatRangeStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAICombatRangeState::EEAICRS_None));
				}
				else
				{
					const double DistanceToTarget = (CombatTargetActor->GetActorLocation() - EnemyBase->GetActorLocation()).Size2D();
					if (DistanceToTarget <= EnemyBase->OnGetAttackRadius())
					{
						BlackboardComp->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::EEAIS_Attacking));
						BlackboardComp->SetValueAsEnum(CombatRangeStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAICombatRangeState::EEAICRS_ShortRange));

					}
					else if (DistanceToTarget <= EnemyBase->OnGetLongAttackRadius())
					{
						BlackboardComp->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::EEAIS_Attacking));
						BlackboardComp->SetValueAsEnum(CombatRangeStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAICombatRangeState::EEAICRS_LongRange));
					}
					else
					{
						BlackboardComp->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::EEAIS_Chase));
						BlackboardComp->SetValueAsEnum(CombatRangeStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAICombatRangeState::EEAICRS_None));
					}

				}

			}
		}
		else
		{
			// CombatTargetはあるが、目視できていない場合
			// 見失った場所へ向かう、周囲を見渡す、などのLostTarget状態へ遷移
			BlackboardComp->SetValueAsEnum(AIStateKey.SelectedKeyName, static_cast<uint8>(EEnemyAIState::EEAIS_LostTarget));
		}

	}

}


