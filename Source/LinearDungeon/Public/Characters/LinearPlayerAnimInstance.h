#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
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

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ALinearPlayerCharacter> LinearPlayerCharacter;

	UPROPERTY(BlueprintReadOnly, category = Movement)
	TObjectPtr<UCharacterMovementComponent> LinearPlayerCharacterMovement;

	UPROPERTY(BlueprintReadOnly, category = Movement)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, category = Movement)
	bool IsFalling;

};
