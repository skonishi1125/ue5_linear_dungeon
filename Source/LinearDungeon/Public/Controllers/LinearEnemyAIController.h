#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionComponent.h" // FAIStimulus は struct なので、.h で定義する

#include "LinearEnemyAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class LINEARDUNGEON_API ALinearEnemyAIController : public AAIController
{
	GENERATED_BODY()
public:
	ALinearEnemyAIController();

	// 死亡時、Enemy から AI Controller の処理を止めるための public 関数
	void HandleEnemyDeath();
protected:
	virtual void BeginPlay() override;

	// キャラクターへの憑依時に呼び出される関数
	virtual void OnPossess(APawn* InPawn) override;

	// AI Perception 感知時に走らせる関数（EnemyBase から移行）
	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus Stimulus);
private:
	// ===== Components =====
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAISenseConfig_Sight> SightConfig;

	// 使用する Behavior Tree の格納先
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTreeAsset;

	// 敵を見失った時の猶予時間 Timer と、呼び出す関数
	FTimerHandle LoseTargetTimer;
	UFUNCTION()
	void ClearCombatTarget();

	UFUNCTION()
	void ResetCharacterDie();

	
};
