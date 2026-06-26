#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "Characters/CharacterTypes.h"

#include "EnemyBase.generated.h"

// AM 関連
class UAnimMontage;
class USoundBase;

// Particle
class UParticleSystem;

// Component
class UAttributeComponent;
class UOverheadStatusWidgetComponent;

// BT, BB 等の呼び出し用
class ALinearEnemyAIController;

// 攻撃
class UBoxComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAttackEndDelegate);

UCLASS()
class LINEARDUNGEON_API AEnemyBase : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemyBase();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;

	FORCEINLINE UAttributeComponent* GetAttributeComponent() const { return Attributes; }

	// Interface の Override
	virtual void GetHit_Implementation(
		const FVector& ImpactPoint, const float FinalPoiseDamage
	) override;

	// AActor の持つ TakeDamage Override
	float TakeDamage(
		float DamageAmount, struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, AActor* DamageCauser
	) override;

	// UAnimNotifyState_AttackCollision などから呼び出す、攻撃判定管理関数
	void OnAttackCollisionNotifyBegin(
		EAttackCollisionType CollisionType,
		float DamageMultiplier = 1.0f, float PoiseMultiplier = 1.0f
	);
	void OnAttackCollisionNotifyEnd();

	// UAnimNotify_EnemyAttackEnd などから呼び出す、攻撃が終わったことを通知する関数
	void OnAttackEnd();
	FOnAttackEndDelegate OnAttackEndDelegate; // BT 側でキャッチするための AttackEnd デリゲート

	// UAnimNotifyState_TrackingTarget から呼び出す、どの区間の間 Target に向かせるかを制御する関数
	void OnTrackingTarget(bool bIsTracking);

	// 巡回ポイントを、Behavior Tree 等から呼び出すための関数
	AActor* OnGetNextPatrolTarget();
	double OnGetAttackRadius() const { return AttackRadius; }
	void OnPerformAttack();

	// 怯みモーションから出す、スタン区間通知用
	void OnStaggerEnd();

protected:
	virtual void BeginPlay() override;

	// 死亡処理
	void Die();
	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive; // ポーズの種類（ABP Idle -> Dead 遷移に使う）

	// 攻撃判定 Overlap 時のイベント関数 (UFUNCTION 必須)
	UFUNCTION()
	void OnRightHandOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnRightLegOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnLeftHandOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
		bool bFromSweep, const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnLeftLegOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult
	);

	void ActivateAttackCollision(EAttackCollisionType CollisionType);
	void DeactivateAttackCollision();

	// ===== Montages =====
	void PlayHitReactionMontage(const FName& SectionName);

	// BoxTrace 判定で考慮しない Actor の配列
	TArray<AActor*> BoxIgnoreActors;

private:
	// 被弾時のベクトル計算
	void DirectionalHitReact(const FVector& ImpactPoint);

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f; // Enemy が攻撃モーションに入るまでの範囲

	// ===== Components =====
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> Attributes; // HP 等の情報
	//UPROPERTY(VisibleAnywhere)
	//TObjectPtr<UHealthBarComponent> HealthBarComponent; // 敵情報 HUD
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UOverheadStatusWidgetComponent> OverheadStatusWidgetComponent; // 頭上に出すための Component

	// ===== Montages =====
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> HitReactionMontage;

	UPROPERTY(EditAnywhere, Category = Sounds)
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = Particles)
	TObjectPtr<UParticleSystem> HitParticle;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> DeathMontage;

	// TArray として 攻撃パターンを持たせることで、敵に応じて色々な攻撃パターンを持たせられるようにする
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TArray<UAnimMontage*> AttackMontages;

	// BT 用
	TObjectPtr<ALinearEnemyAIController> CachedAIController;

	// ===== 徘徊処理(Patrol) =====
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TObjectPtr<AActor> PatrolTarget;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	// ===== 攻撃処理 =====
	// BoxTrace 共通処理
	void PerformAttackTrace(USceneComponent* TraceStart, USceneComponent* TraceEnd, const FString& DebugSocketName);

	// Collision
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> RightHandCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> RightLegCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> LeftHandCollision;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> LeftLegCollision;

	// BoxTrace 始点 / 終点
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> RightHandBoxTraceStart;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> RightHandBoxTraceEnd;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> RightLegBoxTraceStart;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> RightLegBoxTraceEnd;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> LeftHandBoxTraceStart;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> LeftHandBoxTraceEnd;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> LeftLegBoxTraceStart;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> LeftLegBoxTraceEnd;

	// Player 追跡
	bool bIsTrackingTarget = false;
	UPROPERTY(EditAnywhere, Category = "Combat")
	double TrackingInterpSpeed = 5.0;
	void UpdateTrackingRotation(float DeltaTime);

	// 攻撃パラメータ
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float BaseDamage = 20.f; // 基礎攻撃力
	float CurrentDamageMultiplier = 1.0f; // 攻撃倍率
	UPROPERTY(EditAnywhere, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	float BasePoiseDamage = 20.f; // 基礎ポイズ値
	float CurrentPoiseMultiplier = 1.0f; // ポイズ倍率

	// Character の Die デリゲートに紐づける関数
	UFUNCTION()
	void ResetCharacterDie();



};
