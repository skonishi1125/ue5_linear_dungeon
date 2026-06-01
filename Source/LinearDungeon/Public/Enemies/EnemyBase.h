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
class UHealthBarComponent;

// Navigation
class AAIController;

UCLASS()
class LINEARDUNGEON_API AEnemyBase : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemyBase();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Interface の Override
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	// AActor の持つ TakeDamage Override
	float TakeDamage(
		float DamageAmount, struct FDamageEvent const& DamageEvent, 
		class AController* EventInstigator, AActor* DamageCauser
	) override;


protected:
	virtual void BeginPlay() override;

	void Die();
	bool InTargetRange(AActor* Target, double Radius);
	AActor* ChoosePatrolTarget();
	void MoveToTarget(AActor* Target);

	// 死亡ポーズの種類（ABP Idle -> Dead 遷移に使う）
	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	// ===== Montages =====
	void PlayHitReactionMontage(const FName& SectionName);

private:
	// 被弾時のベクトル計算
	void DirectionalHitReact(const FVector& ImpactPoint);

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AActor> CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	// ===== Components =====
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> Attributes; // HP 等の情報

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarWidget; // 体力バー Widget

	// ===== Montages =====
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> HitReactionMontage;

	UPROPERTY(EditAnywhere, Category = Sounds)
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = Particles)
	TObjectPtr<UParticleSystem> HitParticle;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> DeathMontage;

	// ===== 徘徊処理(Patrol) =====
	void CheckPatrolTarget();
	void CheckCombatTarget();
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TObjectPtr<AActor> PatrolTarget;
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;
	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();
	double PatrolRadius = 200.f;
	UPROPERTY(EditAnywhere)
	double PatrolWaitingTime = 3.f;


	
	TObjectPtr<AAIController> EnemyController;

};
