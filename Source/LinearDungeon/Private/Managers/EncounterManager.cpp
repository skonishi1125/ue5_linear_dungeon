#include "Managers/EncounterManager.h"
#include "Logging/StructuredLog.h"

#include "Enemies/EnemyBase.h"
#include "Subsystems/LinearEventSubsystem.h" 
#include "Subsystems/LinearFlagSubsystem.h"
#include "Kismet/GameplayStatics.h"

#include "Interfaces/MovingDoorInterface.h"

AEncounterManager::AEncounterManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void AEncounterManager::BeginPlay()
{
	Super::BeginPlay();

	// 割り振られた EncounterID がクリア済みイベントかチェック
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULinearFlagSubsystem* FlagSubsystem = GI->GetSubsystem<ULinearFlagSubsystem>())
		{
			if (FlagSubsystem->CheckFlag(EncounterID))
			{
				//UE_LOGFMT(LogTemp, Warning, "EncounterManager: {0} is already cleared. Skipping setup.", EncounterID);

				for (AEnemyBase* Enemy : TargetEnemies)
				{
					if (Enemy)
					{
						Enemy->Destroy();
					}
				}

				if (TargetDoor)
				{
					if (TargetDoor->Implements<UMovingDoorInterface>())
					{
						IMovingDoorInterface::Execute_PlayOpen(TargetDoor, true);
					}
					else
					{
						// Interface 未実装のドアは アニメなどはなく Destroy するだけ
						TargetDoor->Destroy();
					}
				}

				return; // 監視処理をスキップ
			}
		}
	}

	// 未クリア
	// 登録した Enemy 1体１体に、HandleEnemyDied() を購読
	RemainingEnemiesCount = TargetEnemies.Num();
	for (AEnemyBase* Enemy : TargetEnemies)
	{
		if (Enemy)
		{
			Enemy->OnEnemyDied.AddDynamic(this, &AEncounterManager::HandleEnemyDied);
		}
	}

}

void AEncounterManager::HandleEnemyDied(AEnemyBase* DeadEnemy)
{
	RemainingEnemiesCount--;
	//UE_LOGFMT(LogTemp, Warning, "AEncounterManager::HandleEnemyDied() Count: {0}", RemainingEnemiesCount);

	if (RemainingEnemiesCount <= 0)
	{
		if (TargetDoor)
		{
			if (TargetDoor->Implements<UMovingDoorInterface>())
			{
				IMovingDoorInterface::Execute_PlayOpen(TargetDoor, false);
			}
			else
			{
				TargetDoor->Destroy();
			}
		}

		// クリア通知
		if (UGameInstance* GI = GetGameInstance())
		{
			if (ULinearFlagSubsystem* FlagSubsystem = GI->GetSubsystem<ULinearFlagSubsystem>())
			{
				FlagSubsystem->SetFlag(EncounterID);
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("All enemies defeated. TargetDoor destroyed."));
	}
}

