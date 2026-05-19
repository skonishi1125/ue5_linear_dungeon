#pragma once

#include "CoreMinimal.h"
#include "Characters/LinearCharacterBase.h"
#include "InputActionValue.h"
#include "Characters/CharacterTypes.h"

#include "LinearPlayerCharacter.generated.h"

// カメラ
class USpringArmComponent;
class UCameraComponent;

// Enhanced Input
class UInputComponent;
class UInputMappingContext;
class UInputAction;

// アイテム関連
class AItemBase;
class FName;

// AM 関連
class UAnimMontage;


UCLASS()
class LINEARDUNGEON_API ALinearPlayerCharacter : public ALinearCharacterBase
{
	GENERATED_BODY()
public:
	ALinearPlayerCharacter();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	FORCEINLINE void SetOverlappingItem(AItemBase* Item) { OverlappingItem = Item; } // inline 関数の強制
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> EquipAction;

	void Equip();


private:
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> Camera;

	// Overlap したアイテム情報の格納
	UPROPERTY(VisibleInstanceOnly) // World に配置した BP_LinearPC でだけ確認できる設定
	TObjectPtr<AItemBase> OverlappingItem;

	FName RightHandSocketName = "RightHandSocket";

	// ======= Anim Montages =======
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> AttackMontage;
	
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TArray<FName> AttackMontageSectionNames{ FName("Attack1"), FName("Attack2") };

};
