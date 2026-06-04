#pragma once

#include "CoreMinimal.h"
#include "Characters/LinearCharacterBase.h"
#include "InputActionValue.h"
#include "Characters/CharacterTypes.h"
#include "Interfaces/HitInterface.h"

#include "LinearPlayerCharacter.generated.h"

// カメラ
class USpringArmComponent;
class UCameraComponent;

// Enhanced Input
class UInputComponent;
class UInputMappingContext;
class UInputAction;

// Health 関連
class UAttributeComponent;

// アイテム関連
class AItemBase;
class AWeapon;
class AShield;
class FName;

// AM 関連
class UAnimMontage;
class USoundBase;

UCLASS()
class LINEARDUNGEON_API ALinearPlayerCharacter : public ALinearCharacterBase, public IHitInterface
{
	GENERATED_BODY()
public:
	ALinearPlayerCharacter();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	FORCEINLINE void SetOverlappingItem(AItemBase* Item) { OverlappingItem = Item; } // inline 関数の強制
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetCharacterActionState() const { return ActionState; }

	// AM_Rolling Notify から呼び出す、FieldSystem 操作関数
	void OnRollingFieldNotifyBegin();
	void OnRollingFieldNotifyEnd();

	// 食らい処理
	float TakeDamage(
		float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser
	) override;
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;

	// AM_Attack Notify から呼び出す、コンボ攻撃用関数
	void OnCanSaveAttack(bool bCanSave);
	void OnCheckCombo();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

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
	bool bCanSaveAttack = false; // 先行入力受付期間かどうか
	bool bSaveAttack = false; // 先行入力が押されたか

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
	void OnAttackAnimEnded();

	// Rolling
	void PlayRollingMontage();
	UFUNCTION(BlueprintCallable)
	void OnRollingAnimEnded();
	void AttachRollingFieldSystem();
	void DetachRollingFieldSystem();

	// HitReaction
	void PlayHitReactionMontage();
	UFUNCTION(BlueprintCallable)
	void OnHitReactionAnimEnded();
	void Die();
	void PlayDeathMontage();
	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive; // 死亡時のポーズ

	// ===== 武器判定操作 =====
	UFUNCTION(BlueprintCallable)
	void OnWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);
	UFUNCTION(BlueprintCallable)
	void OnWeaponCollisionDisabled(ECollisionEnabled::Type CollisionDisabled);



private:
	// ===== Character の状態 =====
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	// Attack Montage 側で Montage 終わりの Notify でこの State を調整するので、BP で公開する
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"));
	EActionState ActionState = EActionState::EAS_Unoccupied;


	// ===== Component =====
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> Attributes; // HP 等

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
	// Attack
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> AttackMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TArray<FName> AttackMontageSectionNames{ 
		FName("ComboAttack1"), FName("ComboAttack2"), FName("ComboAttack3"), FName("ComboAttack4"),
		FName("JumpAttack")
	};
	int32 ComboCountIndex = 0;
	const int32 MaxComboIndex = 4;


	// Rolling
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> RollingMontage;
	UPROPERTY(VisibleInstanceOnly, meta = (AllowPrivateAccess = "true"), Category = "RollingField")
	TObjectPtr<AActor> FieldSystemActor; // BP 側で割り当てる
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "RollingField")
	TSubclassOf<AActor> FieldActorClass;

	// HitReaction
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> HitReactionMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TArray<FName> HitReactionSectionNames{ FName("Impact"), FName("BlockImpact") };
	UPROPERTY(EditAnywhere, Category = Montages)
	TObjectPtr<USoundBase> HitSound;
	UPROPERTY(EditAnywhere, Category = Montages)
	TObjectPtr<USoundBase> BlockSound;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> DeathMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TArray<FName> DeathMontageSectionNames{ FName("Death1"), FName("Death2")};

};
