#include "Controllers/LinearEnemyAIController.h"
#include "Logging/StructuredLog.h"

#include "Characters/LinearPlayerCharacter.h"
#include "Perception/AISenseConfig_Sight.h"// SightConfig

// BehaviorTree
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Enemies/EnemyBase.h"

ALinearEnemyAIController::ALinearEnemyAIController()
{
	// Component 生成処理(EnemyBaseに紐づける)
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	
	// 視覚パラメータの初期設定
	if (SightConfig)
	{
		SightConfig->SightRadius = 1000.f; // 視認可能な半径
		SightConfig->LoseSightRadius = 1100.f; // 見失う半径（チラつき防止のため少し大きめに設定する）
		SightConfig->PeripheralVisionAngleDegrees = 60.f; // 視野角
		SightConfig->SetMaxAge(3.f); // 記憶保持時間

		// どの所属（敵、味方、中立）を検知するか
		// デフォルトでは全て検知しない設定のため、明示的にtrueにする
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

		// Perceptionコンポーネントに設定を登録
		AIPerceptionComponent->ConfigureSense(*SightConfig);
		AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	}
}

void ALinearEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	// 視線に入ったかどうかの処理をデリゲートで紐づける
	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ALinearEnemyAIController::OnTargetDetected);
	}
}

void ALinearEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// 視野角設定取り込み
	if (const AEnemyBase* Enemy = Cast<AEnemyBase>(InPawn))
	{
		ApplySightConfig(Enemy->GetSightSettings());
	}

	// EnemyBase に Controller が憑依成功時、BehaviorTree を起動する
	if (BehaviorTreeAsset)
	{
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

// 視野角関連設定
void ALinearEnemyAIController::ApplySightConfig(const FEnemySightConfig& Settings)
{
	if (!SightConfig || !AIPerceptionComponent) return;

	SightConfig->SightRadius = Settings.SightRadius;
	SightConfig->LoseSightRadius = Settings.LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = Settings.PeripheralVisionAngleDegrees;
	SightConfig->SetMaxAge(Settings.MaxAge);
	// Affiliation はロジック側の共通設定なので constructor のままでOK
	// 変更を反映するため再登録する
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->RequestStimuliListenerUpdate();
}


// 視覚検知処理
// AI Controller に紐づけられた Blackboard に対して、指定の Key に取得した情報を入れる
// BT には BB が紐づいているので、そちらを経由して BB の値を参照できる
void ALinearEnemyAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (Actor->ActorHasTag(ALinearPlayerCharacter::GetTag()))
		{
			// タイマーリセット
			GetWorld()->GetTimerManager().ClearTimer(LoseTargetTimer);


			//UE_LOGFMT(LogTemp, Log, "ALinearEnemyAIController::OnTargetDetected() detect target! : {0}", Actor->GetName());
			// Blackboard コンポーネントを取得し、CombatTarget に Actor をセットする
			if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
			{
				BlackboardComp->SetValueAsObject(FName("CombatTarget"), Actor);
			}

			// Delegate で、Player が死亡したときに CombatTarget をリセットするように登録
			if (ALinearPlayerCharacter* LP_Character = Cast<ALinearPlayerCharacter>(Actor))
			{
				// AddDynamic ではなく AddUniqueDynamic を使って、視界に入るたびに登録されるのを防ぐ
				LP_Character->OnCharacterDeathDelegate.AddUniqueDynamic(this, &ALinearEnemyAIController::OnPlayerCharacterDied);
			}

			// ボスの時は、視線に入った時点で表示を出す
			if (AEnemyBase* Enemy = Cast<AEnemyBase>(GetPawn()))
			{
				if (Enemy->IsBoss())
				{
					Enemy->ShowBossHealthBar();
				}
			}

		}
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(
			LoseTargetTimer, this, &ALinearEnemyAIController::ClearCombatTarget, 6.0f, false
		);
	}
}

void ALinearEnemyAIController::ClearCombatTarget()
{
	if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
	{
		BlackboardComp->ClearValue(FName("CombatTarget"));
		UE_LOGFMT(LogTemp, Log, "ALinearEnemyAIController::ClearCombatTarget() Target forgotten.");
	}

	if (AEnemyBase* Enemy = Cast<AEnemyBase>(GetPawn()))
	{
		Enemy->SetOverheadStatusVisible(false);
	}

}

void ALinearEnemyAIController::HandleEnemyDeath()
{
	if (BrainComponent)
	{
		BrainComponent->StopLogic(TEXT("Dead"));
	}

	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->SetSenseEnabled(UAISense_Sight::StaticClass(), false);
	}

	//UE_LOGFMT(LogTemp, Log, "ALinearEnemyAIController::HandleEnemyDeath() Brain and Perception stopped.");
}

void ALinearEnemyAIController::OnPlayerCharacterDied()
{
	if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
	{
		BlackboardComp->SetValueAsBool(FName("IsTargetDied"), true);
	}
}
