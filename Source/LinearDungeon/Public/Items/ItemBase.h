#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

// Component 関連
class USphereComponent;
class UStaticMeshComponent;

// Overlap 関連
class UPrimitiveComponent;



UCLASS()
class LINEARDUNGEON_API AItemBase : public AActor
{
	GENERATED_BODY()
public:	
	AItemBase();
	//virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

	//UFUNCTION() BP から呼ばないので、不要
	virtual void OnActivatedOverlap();

	virtual void OnDeactivatedOverlap();

private:
	// 派生クラスから nullptr 等に設定されてしまうことを防ぐために private とする
	// BP で触れるようにしつつ, private に配置するために AllowPrivateAccess を設定
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> OverlapSphere; // USceneComponent を継承しているので、これを Root として扱っていく

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ItemMesh;

	UFUNCTION()
	void OnItemBeginOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex
	);

};
