#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NpcPopManager.generated.h"

class UBoxComponent;

UCLASS()
class LINEARDUNGEON_API ANpcPopManager : public AActor
{
	GENERATED_BODY()
	
public:	
	ANpcPopManager();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnBoxBeginOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult
	);

	UFUNCTION()
	void DeleteTarget();

private:
	// SaveData Ç…ï€ë∂Ç∑ÇÈÇΩÇﬂÇÃå≈óL ID
	UPROPERTY(EditInstanceOnly, Category = "Npc|SaveData")
	FName NpcPopID;

	// ä«óùÇ∑ÇÈ NPC îzóÒ
	UPROPERTY(EditInstanceOnly, Category = "Npc|Setup")
	TArray<TObjectPtr<AActor>> TargetNpcs;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> BoxCollision;

	bool bIsDeleted = false;

};
