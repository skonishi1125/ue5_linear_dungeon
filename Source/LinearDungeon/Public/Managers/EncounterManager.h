#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "EncounterManager.generated.h"

class AEnemyBase;

UCLASS()
class LINEARDUNGEON_API AEncounterManager : public AActor
{
	GENERATED_BODY()
	
public:	
	AEncounterManager();

protected:
	virtual void BeginPlay() override;

private:
	// SaveData に保存するための固有 ID
	UPROPERTY(EditInstanceOnly, Category = "Encounter|SaveData")
	FName EncounterID;

	// 管理する敵の配列
	UPROPERTY(EditInstanceOnly, Category = "Encounter|Setup")
	TArray<TObjectPtr<AEnemyBase>> TargetEnemies;

	// 条件達成時、操作する Actor (扉など）
	UPROPERTY(EditInstanceOnly, Category = "Encounter|Setup")
	TObjectPtr<AActor> TargetDoor;

	UFUNCTION()
	void HandleEnemyDied(AEnemyBase* DeadEnemy);

	int32 RemainingEnemiesCount;


};
