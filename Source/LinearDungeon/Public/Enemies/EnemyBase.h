#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"

#include "EnemyBase.generated.h"

// AM 関連
class UAnimMontage;
class USoundBase;

// Particle
class UParticleSystem;

// Component
class UAttributeComponent;
class UHealthBarComponent;

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

	// ===== Montages =====
	void PlayHitReactionMontage(const FName& SectionName);

private:
	// ===== Components =====
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> Attributes; // HP 等の情報

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UHealthBarComponent> HealthBarWidget; // 体力バー Widget

	// 被弾時のベクトル計算
	void DirectionalHitReact(const FVector& ImpactPoint);

	// ===== Montages =====
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> HitReactionMontage;

	UPROPERTY(EditAnywhere, Category = Sounds)
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = Particles)
	TObjectPtr<UParticleSystem> HitParticle;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> DeathMontage;

};
