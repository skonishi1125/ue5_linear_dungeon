#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "PlayerTargetingComponent.generated.h"

class USpringArmComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LINEARDUNGEON_API UPlayerTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerTargetingComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Target ボタンが押された時の処理
	void OnLockOnPressed();

	// SpringArm 初期設定用
	void SetSpringArm(USpringArmComponent* SpringArm);

	void UpdateLockOnCamera(class USpringArmComponent* SpringArm, float DeltaTime); // Player 側 Tick で動かす
	void UpdateTargetWidgetPosition(); //  Player 側 Tick で Marker を Target に追従させる処理
	FORCEINLINE bool IsLocked() const { return bIsLocked; }
	FORCEINLINE AActor* GetCurrentTarget() const { return CurrentTarget.Get(); }

protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<USpringArmComponent> CachedPlayerSpringArm;
	float DefaultArmLength;
	FRotator DefaultArmRotation;
	UPROPERTY(EditAnywhere, Category = "Targeting")
	float MaxLockDistance = 2000.f;
	UPROPERTY(EditAnywhere, Category = "Targeting")
	float LockOnArmLength = 500.f;
	UPROPERTY(EditAnywhere, Category = "Targeting")
	FRotator LockOnArmRotation = FRotator(-20.f, 0.f, 0.f);
	UPROPERTY(EditAnywhere, Category = "Targeting")
	float CameraRotationInterpSpeed = 5.f; // カメラ 回転速度

	// Marker
	UPROPERTY(EditAnywhere, Category = "Targeting")
	TSubclassOf<UUserWidget> TargetMarkerClass;
	TObjectPtr<UUserWidget> TargetMarkerWidget;

private:
	// 索敵して、条件に合う相手を CurrentTarget に入れる処理
	AActor* FindBestTarget() const;
	// TWeakObjectPtr: 弱い参照(いつ破棄されるか予測できない参照を保持するときに使う)
	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentTarget;

	// Target / デフォルト の設定値切り替え用
	void ApplySpringArmProfile();
	void RestoreSpringArmProfile();



	// 距離、死亡状況などをチェックして、ターゲット状態を維持するかを決める関数
	void IsTargetValid();

	// ターゲティングの解除
	void ClearLock();

	bool bIsLocked = false;
};
