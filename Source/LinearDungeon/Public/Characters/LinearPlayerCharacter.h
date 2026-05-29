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
class AWeapon;
class AShield;
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
	FORCEINLINE EActionState GetCharacterActionState() const { return ActionState; }

protected:
	virtual void BeginPlay() override;

	// ===== Enhanced Input 関連メソッド =====
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;
	void Move(const FInputActionValue& Value);
	bool CanMove();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LookAction;
	void Look(const FInputActionValue& Value);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;
	void TryJump();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;
	void Attack();
	bool CanAttack();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> DefenseAction;
	void StartDefense();
	void StopDefense();
	bool CanDefense();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> EquipAction;
	void Equip();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> RollingAction;
	void Rolling();
	bool CanRolling();

	// ===== Montages 関連 =====
	// Attack
	void PlayAttackMontage();
	UFUNCTION(BlueprintCallable)
	void AttackEnd();

	// Rolling
	void PlayRollingMontage();
	UFUNCTION(BlueprintCallable)
	void RollingEnd();

	// ===== 武器判定操作 =====
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionDisabled(ECollisionEnabled::Type CollisionDisabled);



private:
	// ===== Character の状態 =====
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	// Attack Montage 側で Montage 終わりの Notify でこの State を調整するので、BP で公開する
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"));
	EActionState ActionState = EActionState::EAS_Unoccupied;


	// ===== カメラなど紐づけるComponent =====
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> Camera;

	// ===== Overlap したアイテム情報の格納と、割り当てるソケット =====
	UPROPERTY(VisibleInstanceOnly) // World に配置した BP_LinearPC でだけ確認できる設定
	TObjectPtr<AItemBase> OverlappingItem;

	UPROPERTY(VisibleAnywhere, Category = Equipment)
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(VisibleAnywhere, Category = Equipment)
	TObjectPtr<AShield> EquippedShield;

	FName RightHandSocketName = "RightHandSocket";
	FName LeftHandSocketName = "LeftHandSocket";

	// ======= Anim Montages =======
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> AttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TArray<FName> AttackMontageSectionNames{ FName("Attack1"), FName("Attack2") };

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> RollingMontage;

};
