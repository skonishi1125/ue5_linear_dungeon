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
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

	// UFINCTION() は UE のシステムがこの関数にアクセスしてチェックするので、
	// protected 以上の関数で基本的に設定する
	// またオーバーライド想定の関数は、同じように protected 以上で定義しておく
	UFUNCTION()
	virtual void OnItemBeginOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult
	);

	UFUNCTION()
	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex
	);

	// 継承先 Class で ItemMesh->... とするため、protected に配置
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ItemMesh;

private:
	// 派生クラスから nullptr 等に設定されてしまうことを防ぐために private とする
	// BP で触れるようにしつつ, private に配置するために AllowPrivateAccess を設定
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> OverlapSphere; // USceneComponent を継承しているので、これを Root として扱っていく

	// 放置アイテムの浮遊, 回転処理
	float RunningTime;

	void HoveringObject();
	float Amplitude = .1f; // 振幅
	float TimeConstant = 2.f; // 波長 値が大きくなるほど往復スピードが上がる。(波長 = 2π / k における、k)

	void RotateObject(float DeltaTime);
	float RotationSpeed = 90.0f; // [deg/s]: 秒間あたり何度回るか



};
