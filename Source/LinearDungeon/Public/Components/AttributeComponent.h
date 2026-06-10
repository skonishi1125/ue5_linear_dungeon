#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPoisePercentChangedDelegate, float, NewPercent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class LINEARDUNGEON_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Health 関連
	void ReceiveHealthDamage(float Damage);
	float GetHealthPercent() const;
	bool IsAlive() const;

	// Poise 関連
	float GetPoisePercent() const;
	bool IsStaggeredWithPoise(float PoiseDamage);
	void ResetPoise();

	FOnPoisePercentChangedDelegate OnPoisePercentChanged;

protected:
	virtual void BeginPlay() override;

private:
	// Health 関連
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float CurrentHealth;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	// Poise 関連
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float CurrentPoise;
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxPoise;
	void ApplyPoiseDamage(float PoiseDamage);
	void MarkPoiseDamaged();
	void UpdatePoiseRecoveryTickEnabled();
	void BroadcastPoisePercent() const;
	// 被弾後、Poise 回復が始まるまでの待ち時間（秒
	UPROPERTY(EditAnywhere, Category = "Actor Attributes|Poise Recovery")
	float PoiseRegenDelay = 5.f;
	// Max まで戻るまでの時間（秒）。Delay 経過後の回復速度に使う
	UPROPERTY(EditAnywhere, Category = "Actor Attributes|Poise Recovery")
	float PoiseRegenDuration = 5.f;
	float LastPoiseDamageTime = 0.f;

};
