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


protected:
	virtual void BeginPlay() override;

	// ===== Montages =====
	void PlayHitReactionMontage(const FName& SectionName);

private:
	// 被弾時のベクトル計算
	void DirectionalHitReact(const FVector& ImpactPoint);

	// ===== Montages =====
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> HitReactionMontage;

	UPROPERTY(EditAnywhere, Category = Sounds)
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditAnywhere, Category = Particles)
	TObjectPtr<UParticleSystem> HitParticle;

};
