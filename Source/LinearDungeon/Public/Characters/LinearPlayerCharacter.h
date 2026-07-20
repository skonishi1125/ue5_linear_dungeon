#pragma once

#include "CoreMinimal.h"
#include "Characters/LinearCharacterBase.h"
#include "InputActionValue.h"
#include "Characters/CharacterTypes.h"
#include "Interfaces/HitInterface.h"
#include "Interfaces/SaveInterface.h"

#include "LinearPlayerCharacter.generated.h"

// カメラ
class USpringArmComponent;
class UCameraComponent;

// Enhanced Input
class UInputComponent;
class UInputMappingContext;
class UInputAction;

// Health, Potion 関連
class UAttributeComponent;
class UInventoryComponent;

// アイテム関連
class AItemBase;
class AWeapon;
class AShield;
class FName;

// AM 関連
class UAnimMontage;
class USoundBase;

// Particle
class UParticleSystem;

// Die 死亡通知
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterDeathDelegate);

// HUD, Overlay 関連
class ULinearDungeonOverlay;

// ターゲットカメラ
class UPlayerTargetingComponent;

// 会話関連
class ULinearDialogueComponent;
class UInteractMarker;




UCLASS()
class LINEARDUNGEON_API ALinearPlayerCharacter : public ALinearCharacterBase, public IHitInterface, public ISaveInterface
{
	GENERATED_BODY()
public:
	ALinearPlayerCharacter();
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	FORCEINLINE void SetOverlappingItem(AItemBase* Item) { OverlappingItem = Item; } // inline 関数の強制
	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetCharacterActionState() const { return ActionState; }
	FORCEINLINE void SetCharacterActionState(EActionState NewActionState) { ActionState = NewActionState; }
	static FORCEINLINE FName GetTag() { return TagName; }
	FORCEINLINE UAttributeComponent* GetAttributeComponent() const { return Attributes; }
	FORCEINLINE UInventoryComponent* GetInventoryComponent() const { return Inventories; }
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }

	// ===== Interface Override ===== 
	virtual void GetHit_Implementation(
		const FVector& ImpactPoint, const float FinalPoiseDamage, bool bIsParry
	) override;
	virtual void OnSaveGame(ULinearSaveGame* SaveGameObj) override;
	virtual void OnLoadGame(ULinearSaveGame* SaveGameObj) override;

	// インタラクト DialogueComponent 関連
	// ↑の ItemBase と同じ処理だが、 BP に慣れるためにActor を格納処理は BP で 作ってみる
	UFUNCTION(BlueprintCallable)
	void SetOverlappingInteractableActor(AActor* Actor); // BP_Prisoner などの Overlap イベントから呼べるようにしておく
	UFUNCTION(BlueprintCallable)
	void SetActiveDialogueComponent(ULinearDialogueComponent* DialogueComponent); // ↑と同じ
	UFUNCTION()
	void OnDialogueEnd();

	// AM_Rolling Notify から呼び出す、FieldSystem 操作関数
	void OnRollingFieldNotifyBegin();
	void OnRollingFieldNotifyEnd();

	// 食らい処理
	float TakeDamage(
		float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser
	) override;

	// パリィ受付フラグ設定
	void SetParryWindowEnabled(bool bEnabled);
	FORCEINLINE bool IsParryWindowEnabled() { return bIsParryWindowEnabled; }

	// AM_Attack Notify から呼び出す関数
	// 武器の判定有効 - 無効化
	void OnWeaponCollisionEnabled(
		ECollisionEnabled::Type CollisionEnabled, float DamageMultiplier, float PoiseMultiplier
	);
	void OnWeaponCollisionDisabled(ECollisionEnabled::Type CollisionDisabled);
	// コンボ攻撃用関数
	void OnCanSaveAttack(bool bCanSave); // ComboWindow 先行入力受付フラグ操作関数
	void OnCheckCombo();// CheckCombo 先行入力を受け付けたとき、次の Montage_Play を実行する関数
	void OnAttackSteering(bool bCanSteer); // コンボ中のステアリング制御

	// 死亡処理
	// デリゲート
	FOnCharacterDeathDelegate OnCharacterDeathDelegate;
	// 落下時など、外から Player の死亡を呼ぶことができる関数
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ForceKill(EDeathCause Cause);

	// InteractInterface 実行時から盾 / 剣等から呼べるように、public で装備処理を用意しておく
	void EquipWeapon(AWeapon* InWeapon, bool bPlaySound = true);
	void EquipShield(AShield* InShield, bool bPlaySound = true);

	FVector GetEquipmentDropLocation() const;

	// ポーション使用中、移動速度を調整するためのステアリング
	// ANS_ModifyMoveSpeed などから呼ぶ
	void OnUsePotionSteering(bool bCanSteer);

	// Level Sequence 再生前 / 後で呼び出すセッタ関数
	UFUNCTION(BlueprintCallable)
	void SetInCinematic(bool bInCinematic);

	// ===== Debug =====
	//UFUNCTION(Exec)
	//void DebugSaveGame();
	//UFUNCTION(Exec)
	//void DebugLoadGame();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// ===== Enhanced Input 関連メソッド =====
	// 基本 IMC
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	// Dialogue 表示時 IMC
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DialogueMappingContext;

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
	bool bCanAttackSteering = false; // 攻撃中の方向微調整・微前進の許可フラグ

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> DefenseAction;
	void StartDefense();
	void StopDefense();
	bool CanDefense();
	bool bIsParryWindowEnabled = false; // パリィ受付

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;
	void Interact();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> EvadeSprintAction;
	void EvadeSprintStarted();
	void EvadeSprintTriggered();
	void EvadeSprintCompleted();
	void StartSprint();
	void StopSprint();
	bool CanSprint();
	void Rolling();
	bool CanRolling();
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DefaultSpeed = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DashSpeed = 850.f;


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> TargetAction;
	void Target();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> AdvanceDialogueAction;
	void AdvanceDialogue();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> UsePotionAction;
	void UsePotion();
	bool CanUsePotion();
	bool bCanUsePotionSteering = false; // アイテム使用中の方向微調整・微前進の許可フラグ

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
	void PlayHardHitReactionMontage();
	void PlayDefenseReactionMontage();
	// Hard Staggerに移行するための超過ダメージの閾値
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HardStaggerThreshold = 20.f;
	UFUNCTION(BlueprintCallable)
	void OnHitReactionAnimEnded();
	void Die(EDeathCause Cause = EDeathCause::EDC_Normal);
	void PlayDeathMontage(EDeathCause Cause);
	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive; // 死亡時のポーズ
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	EDeathCause CurrentDeathCause = EDeathCause::EDC_Normal;

	// ===== Interact 関連 =====
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UInteractMarker> InteractMarkerClass;

private:
	// ===== Character 設定 =====
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	// Attack Montage 側で Montage 終わりの Notify でこの State を調整するので、BP で公開する
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"));
	EActionState ActionState = EActionState::EAS_Unoccupied;

	// タグ
	static const FName TagName;

	// ===== Component =====
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> DeathSpringArm;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> DeathCamera;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAttributeComponent> Attributes; // HP 等
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInventoryComponent> Inventories;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPlayerTargetingComponent> PlayerTargeting;

	// ===== SpringArm/Camera 設定値 =====
	UPROPERTY(EditAnywhere, Category = "Camera")
	float DefaultArmLength = 400.f;
	UPROPERTY(EditAnywhere, Category = "Camera")
	FRotator DefaultArmRotation = FRotator(-20.f, 0.f, 0.f);


	// ===== Overlap したアイテム情報の格納と、割り当てるソケット =====
	UPROPERTY(VisibleInstanceOnly) // World に配置した BP_LinearPC でだけ確認できる設定
	TObjectPtr<AItemBase> OverlappingItem;

	// 武器
	UPROPERTY(VisibleAnywhere, Category = Equipment)
	TObjectPtr<AWeapon> EquippedWeapon;
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<AWeapon> EquippedWeaponClass; // Load 時、装備している武器をここから生成して装備させる
	
	// 盾
	UPROPERTY(VisibleAnywhere, Category = Equipment)
	TObjectPtr<AShield> EquippedShield;
	UPROPERTY(VisibleAnywhere)
	TSubclassOf<AWeapon> EquippedShieldClass;
	
	UPROPERTY(VisibleAnywhere, Category = Equipment)
	FName RightHandSocketName = "RightHandSocket";
	UPROPERTY(VisibleAnywhere, Category = Equipment)
	FName LeftHandSocketName = "LeftHandSocket";

	// ===== Overlap している Interactable な Actor 情報を格納する変数
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<AActor> OverlappingInteractableActor;

	// ======= Anim Montages =======
	// Attack
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TArray<UAnimMontage*> ComboMontages;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TArray<UAnimMontage*> ComboHalberdMontages;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> JumpAttackMontage;
	int32 ComboCountIndex = 0;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Attack")
	float AttackSteeringMultiply = .15f;


	// Rolling
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> RollingMontage;
	UPROPERTY(VisibleInstanceOnly, meta = (AllowPrivateAccess = "true"), Category = "RollingField")
	TObjectPtr<AActor> FieldSystemActor; // BP 側で割り当てる
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "RollingField")
	TSubclassOf<AActor> FieldActorClass;

	bool bHasSprintStarted = false;

	// HitReaction
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> HitReactionMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> HardHitReactionMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> DefenseReactionMontage;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"), Category = "Defense")
	float DefenseMultiply = .3f; // 防御時、HP / Poise にかける補正値

	UPROPERTY(EditAnywhere, Category = Montages)
	TObjectPtr<USoundBase> HitSound;
	UPROPERTY(EditAnywhere, Category = Particles)
	TObjectPtr<UParticleSystem> HitParticle;
	UPROPERTY(EditAnywhere, Category = Montages)
	TObjectPtr<USoundBase> BlockSound;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> DeathMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TArray<FName> DeathMontageSectionNames{ FName("Death1"), FName("Death2")};
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> FallDeathMontage;
	UPROPERTY(EditAnywhere, Category = Montages)
	TObjectPtr<USoundBase> FallDeathVoice;

	// UsePotion
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	TObjectPtr<UAnimMontage> UsePotionReactionMontage;
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"), Category = "Use Potion")
	float UsePotionSteeringMultiply = .6f;

	// ===== HUD, Overlay 関連 =====
	void CacheLinearDungeonOverlay();
	UPROPERTY()
	TObjectPtr<ULinearDungeonOverlay> LinearDungeonOverlay;
	void BindOverlayToAttributes();
	UFUNCTION()
	void OnHealthPercentChanged(float NewPercent);
	UFUNCTION()
	void OnPoisePercentChanged(float NewPercent);
	UFUNCTION()
	void OnNumberOfPotionChanged(int32 NewNumberOfPotion);

	// ===== Interact 関連 =====
	UPROPERTY()
	TObjectPtr<class UInteractMarker> InteractMarkerWidget; // 生成した Widget 保持用
	// Dialogue Text 関連
	// 現在アクティブなダイアログ表示の情報
	// Overlap されたものを Interact() する以外でも、ムービーシーン等でもダイアログは発生する
	// そういったものも含めて、今処理すべきダイアログを格納したもの
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<ULinearDialogueComponent> ActiveDialogueComponent;

	// ===== イベントシーン =====
	bool bIsInCinematic = false; // Level Sequence 再生中かどうかのフラグ

};
