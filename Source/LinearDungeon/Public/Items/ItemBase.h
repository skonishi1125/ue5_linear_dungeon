#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

// Component 関連
class USphereComponent;
class UStaticMeshComponent;

// Overlap 関連
class UPrimitiveComponent;

enum class EItemState : uint8
{
	EIS_Dropped,
	EIS_Equipped
};


UCLASS()
class LINEARDUNGEON_API AItemBase : public AActor
{
	GENERATED_BODY()
public:	
	AItemBase();
	virtual void Tick(float DeltaTime) override;
protected:
	// ===== State 関連 =====
	EItemState ItemState = EItemState::EIS_Dropped;

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

	// 派生クラスから nullptr 等に設定されてしまうことを防ぐために private とする
	// BP で触れるようにしつつ, private に配置するために AllowPrivateAccess を設定
	// -> 26.5.20 元々 private だったが、Weapon 側で 装備時に Collision 設定を無効とするため protected に配置
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> OverlapSphere; // USceneComponent を継承しているので、これを Root として扱っていく

	// 継承先 Class で ItemMesh->... とするため、protected に配置
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ItemMesh;

private:
	// 放置アイテムの浮遊, 回転処理
	float RunningTime;

	void HoveringObject();
	float Amplitude = .1f; // 振幅
	float TimeConstant = 2.f; // 波長 値が大きくなるほど往復スピードが上がる。(波長 = 2π / k における、k)

	void RotateObject(float DeltaTime);
	float RotationSpeed = 90.0f; // [deg/s]: 秒間あたり何度回るか



};
