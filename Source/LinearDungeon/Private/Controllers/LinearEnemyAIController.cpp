#include "Controllers/LinearEnemyAIController.h"
#include "Logging/StructuredLog.h"

#include "Characters/LinearPlayerCharacter.h"
#include "Perception/AISenseConfig_Sight.h"// SightConfig

// BehaviorTree
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "Enemies/EnemyBase.h"
#include "Subsystems/LinearAudioSubsystem.h"
#include "GameMode/LinearGameMode.h"

ALinearEnemyAIController::ALinearEnemyAIController()
{
	// Component 生成処理(EnemyBaseに紐づける)
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("InitSightConfig"));
	
	// 視覚パラメータの初期設定
	if (SightConfig)
	{
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

	// 視線に入った / MaxAge が経過し、記憶から消えたときに走る Delegate
	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &ALinearEnemyAIController::OnTargetDetected);
		AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &ALinearEnemyAIController::OnTargetForgotten);

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
	if (! AIPerceptionComponent || !SightConfig) return;


	SightConfig->SightRadius = Settings.SightRadius;
	SightConfig->LoseSightRadius = Settings.LoseSightRadius;
	SightConfig->PeripheralVisionAngleDegrees = Settings.PeripheralVisionAngleDegrees;
	SightConfig->SetMaxAge(Settings.MaxAge);
	// Affiliation はロジック側の共通設定なので constructor のままでよい。変更を反映するため再登録する
	AIPerceptionComponent->ConfigureSense(*SightConfig);
	AIPerceptionComponent->RequestStimuliListenerUpdate();
}


// 視覚検知処理
// AI Controller に紐づけられた Blackboard に対して、指定の Key に取得した情報を入れる
// BT には BB が紐づいているので、そちらを経由して BB の値を参照できる
void ALinearEnemyAIController::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	// Player 以外は考慮しない
	if (!IsValid(Actor) || !Actor->ActorHasTag(ALinearPlayerCharacter::GetTag())) return;

	// 視覚以外は考慮しない
	if (Stimulus.Type != UAISense::GetSenseID<UAISense_Sight>()) return;

	UBlackboardComponent* BlackboardComp = GetBlackboardComponent();
	if (!BlackboardComp) return;

	if (Stimulus.WasSuccessfullySensed())
	{
		UE_LOGFMT(LogTemp, Warning, "ALinearEnemyAIController::OnTargetDetected() detect target! : {0}", Actor->GetName());

		if (SightConfig)
			UE_LOGFMT(LogTemp, Warning, "MaxAge: {0}", SightConfig->GetMaxAge());

		// Behavior Tree 制御用設定
		// BB 内部の CombatTarget に Actor を, 視線が通っていることを示すフラグを有効化
		BlackboardComp->SetValueAsObject(FName("CombatTarget"), Actor);
		BlackboardComp->SetValueAsBool(FName("HasLineOfSight"), true);

		// Delegate で、Player が死亡したときに CombatTarget をリセットするように登録
		// AddDynamic ではなく AddUniqueDynamic で、視界に入るたびに登録されるのを防ぐ
		if (ALinearPlayerCharacter* LP_Character = Cast<ALinearPlayerCharacter>(Actor))
		{
			LP_Character->OnCharacterDeathDelegate.AddUniqueDynamic(this, &ALinearEnemyAIController::OnPlayerCharacterDied);
		}

		// ボスの時は、視線に入った時点で表示を出す
		if (AEnemyBase* Enemy = Cast<AEnemyBase>(GetPawn()))
		{
			if (Enemy->IsBoss())
			{
				Enemy->ShowBossHealthBar();

				// BGM 再生(重複チェックなどは GameMode, AudioSubsystem に任せる)
				// 本当は BGM, UI 表示なども別途委託したほうが良いが、ボスが 1 体なのでここで設計する
				if (ALinearGameMode* GameMode = GetWorld()->GetAuthGameMode<ALinearGameMode>())
				{
					GameMode->ChangeBGM(EBGMType::Boss);
				}
			}
		}
	}
	else
	{
		// 視界から外れたときの処理
		BlackboardComp->SetValueAsBool(FName("HasLineOfSight"), false);
	}
}

// MaxAge 経過後処理(視点が外れた時点で発火するわけではない)
// 経過後に Delegate 経由でこの関数が走る

void ALinearEnemyAIController::OnTargetForgotten(AActor* Actor)
{
	if (!IsValid(Actor) || !Actor->ActorHasTag(ALinearPlayerCharacter::GetTag())) return;

	ClearCombatTarget();
}

void ALinearEnemyAIController::ClearCombatTarget()
{
	if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
	{
		BlackboardComp->ClearValue(FName("CombatTarget"));
		UE_LOGFMT(LogTemp, Warning, "ALinearEnemyAIController::ClearCombatTarget() Target forgotten.");
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

void ALinearEnemyAIController::ChangeAIState(EEnemyAIState NewState)
{
	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		BB->SetValueAsEnum(FName("CurrentEnemyAIState"), static_cast<uint8>(NewState));
	}
}

void ALinearEnemyAIController::OnPlayerCharacterDied()
{
	if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
	{
		BlackboardComp->SetValueAsBool(FName("IsTargetDied"), true);
	}
}
