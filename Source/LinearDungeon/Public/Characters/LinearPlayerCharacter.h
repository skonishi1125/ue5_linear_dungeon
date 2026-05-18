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

// アイテム関連
class AItemBase;
class FName;

// UE における Enum の作法として、E...という形で定義する
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon"),
};

UCLASS()
class LINEARDUNGEON_API ALinearPlayerCharacter : public ALinearCharacterBase
{
	GENERATED_BODY()
public:
	ALinearPlayerCharacter();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	FORCEINLINE void SetOverlappingItem(AItemBase* Item) { OverlappingItem = Item; } // inline void の強制


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

};
