#pragma once

#include "CoreMinimal.h"
#include "Characters/LinearCharacterBase.h"
#include "InputActionValue.h"

#include "LinearPlayerCharacter.generated.h"

// カメラ
class USpringArmComponent;
class UCameraComponent;

// Enhanced Input
class UInputComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class LINEARDUNGEON_API ALinearPlayerCharacter : public ALinearCharacterBase
{
	GENERATED_BODY()
public:
	ALinearPlayerCharacter();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	// ===== Enhanced Input 関連メソッド =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	void Move(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	void Look(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;

	void TryJump();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

	void Attack();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> Camera;

};
