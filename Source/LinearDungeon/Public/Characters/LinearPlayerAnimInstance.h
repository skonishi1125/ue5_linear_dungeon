#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Characters/CharacterTypes.h"

#include "LinearPlayerAnimInstance.generated.h"

class ALinearPlayerCharacter;
class UCharacterMovementComponent;

UCLASS()
class LINEARDUNGEON_API ULinearPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	void SetLeftHandIKAlphaMultiplierTarget(float NewAlphaMultiplier);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ALinearPlayerCharacter> LinearPlayerCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	TObjectPtr<UCharacterMovementComponent> LinearPlayerCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsFalling;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool bIsDefensing;

	UPROPERTY(BlueprintReadOnly, Category = "Movement | Character State")
	ECharacterState CharacterState;

	UPROPERTY(BlueprintReadOnly, Category = "IK")
	FVector LeftHandIKLocation;
	UPROPERTY(BlueprintReadOnly, Category = "IK")
	float LeftHandIKAlpha = 0.f;
	// 実際に今使っている倍率
	float LeftHandIKAlphaMultiplier = 1.f;
	// NotifyState から指定される目標倍率
	float LeftHandIKAlphaMultiplierTarget = 1.f;
	// 目標倍率へ近づく速さ
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "IK")
	float LeftHandIKInterpSpeed = 8.f;

};
