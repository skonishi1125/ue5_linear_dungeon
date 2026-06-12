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

	// PlayerCharacter から 各情報は読みたいので、public にする
	void UpdateLockOnCamera(class USpringArmComponent* SpringArm, float DeltaTime);
	FORCEINLINE bool IsLocked() const { return bIsLocked; }
	FORCEINLINE AActor* GetCurrentTarget() const { return CurrentTarget.Get(); }

	// SpringArm 初期設定用
	void SetSpringArm(USpringArmComponent* SpringArm);

protected:
	virtual void BeginPlay() override;

	AActor* FindBestTarget() const;


	void ApplySpringArmProfile();
	void RestoreSpringArmProfile();
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
	float CameraZoomInterpSpeed = 5.f; // カメラ ズーム速度
	UPROPERTY(EditAnywhere, Category = "Targeting")
	float CameraRotationInterpSpeed = 5.f; // カメラ 回転速度

private:
	bool bIsLocked = false; // ロック中かどうかを示す

	// TWeakObjectPtr: 弱い参照
	// いつ破棄されるか予測できない Actor の参照を保持するときに使う。
	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentTarget;

	void ClearLock();
	void IsTargetValid();
		
};
