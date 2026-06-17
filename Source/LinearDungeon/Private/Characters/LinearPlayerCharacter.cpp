#include "Characters/LinearPlayerCharacter.h"
#include "Logging/StructuredLog.h"

// Component
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BoxComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PlayerTargetingComponent.h"

// 扱う Actor
#include "Items/ItemBase.h"
#include "Items/Weapon.h"
#include "Items/Shield.h"
#include "Animation/AnimMontage.h"

// 会話関連
#include "Interfaces/InteractInterface.h"
#include "Components/LinearDialogueComponent.h"

// 音関連
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

// HUD
#include "Components/HUD/LinearDungeonHUD.h"
#include "Components/HUD/LinearDungeonOverlay.h"

// Save 機能
#include "Subsystems/LinearSaveSubsystem.h"
#include "Controllers/LinearPlayerController.h"
#include "SaveGames/LinearSaveGame.h"

const FName ALinearPlayerCharacter::TagName = FName(TEXT("LinearPlayerCharacter"));

ALinearPlayerCharacter::ALinearPlayerCharacter()
{
	// Controller の回転に Character 自身は依存しないことを明示する
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// 進行した方向にキャラクターが振り向く設定
	// ACharacter が CharacterMovementComponent を生成済みなので、nullptr チェック不要
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	// Component
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmBlankCharacter"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = DefaultArmLength;
	SpringArm->bUsePawnControlRotation = true; // Controler の回転を SpringArmに反映させる
	SpringArm->SetRelativeRotation(DefaultArmRotation);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	PlayerTargeting = CreateDefaultSubobject<UPlayerTargetingComponent>(TEXT("PlayerTargeting"));

}

void ALinearPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add(TagName);
	UE_LOGFMT(LogTemp, Warning, "Attached Tags. Name: {0}", ALinearPlayerCharacter::GetTag());

	if (PlayerTargeting)
	{
		PlayerTargeting->SetSpringArm(SpringArm);
	}

	CacheLinearDungeonOverlay();
	BindOverlayToAttributes();
}

// Overlay をメンバ変数に格納する
void ALinearPlayerCharacter::CacheLinearDungeonOverlay()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		ALinearDungeonHUD* LinearDungeonHUD = Cast<ALinearDungeonHUD>(PlayerController->GetHUD());
		if (LinearDungeonHUD)
		{
			LinearDungeonOverlay = LinearDungeonHUD->GetOverlay();
		}
	}
}

void ALinearPlayerCharacter::BindOverlayToAttributes()
{
	if (Attributes)
	{
		// Health / Poise 値が削れた時に反応する Delegate
		// Remove は安全な処理なので、多重登録を防ぐために Remove を一度しておく形をとる
		// Attributes->OnPoisePercentChanged.RemoveDynamic(this, &ALinearPlayerCharacter::OnPoisePercentChanged);
		// Attributes->OnPoisePercentChanged.AddDynamic(this, &ALinearPlayerCharacter::OnPoisePercentChanged);
		// ↑でもいいが、楽にやるなら UniqueDynamic を使う

		Attributes->OnHealthPercentChanged.AddUniqueDynamic(this, &ALinearPlayerCharacter::OnHealthPercentChanged);
		Attributes->OnPoisePercentChanged.AddUniqueDynamic(this, &ALinearPlayerCharacter::OnPoisePercentChanged);
		
		// Poise 初期値の % を渡して通知して反映
		OnHealthPercentChanged(Attributes->GetHealthPercent());
		OnPoisePercentChanged(Attributes->GetPoisePercent());
	}
}

// Health, Poise が Attribute で変動があったとき、
// Overlay 側にも変更値を反映させる
// ※本クラス Character が仲介しているが、理想は Overlay 側だけで済ませたほうが疎結合になる
void ALinearPlayerCharacter::OnHealthPercentChanged(float NewPercent)
{
	if (LinearDungeonOverlay)
	{
		LinearDungeonOverlay->SetHealthBarPercent(NewPercent);
	}
}

void ALinearPlayerCharacter::OnPoisePercentChanged(float NewPercent)
{
	if (LinearDungeonOverlay)
	{
		LinearDungeonOverlay->SetPoiseBarPercent(NewPercent);
	}
}

void ALinearPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ロック中、カメラを敵に準拠した動きにする
	if (PlayerTargeting && PlayerTargeting->IsLocked())
	{
		PlayerTargeting->UpdateLockOnCamera(SpringArm, DeltaTime);
		PlayerTargeting->UpdateTargetWidgetPosition();
	}
}

void ALinearPlayerCharacter::SetOverlappingInteractableActor(AActor* Actor)
{
	OverlappingInteractableActor = Actor;
}

void ALinearPlayerCharacter::SetActiveDialogueComponent(ULinearDialogueComponent* DialogueComponent)
{
	ActiveDialogueComponent = DialogueComponent;

	// このセッタが呼ばれる = 会話が始まったということなので、IMC も切り替える
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (ActiveDialogueComponent)
			{
				// 優先度 1 で登録して、DefaultMappingContext の 0 より 優先度を上げておく
				Subsystem->AddMappingContext(DialogueMappingContext, 1);
			}
			else
			{
				// 無い場合は外しておく
				Subsystem->RemoveMappingContext(DialogueMappingContext);
			}
		}
	}
}


void ALinearPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// IMC 登録
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	// Action と対応するメソッドとのバインド
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)
		{
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALinearPlayerCharacter::Move);
		}

		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ALinearPlayerCharacter::Look);
		}

		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ALinearPlayerCharacter::TryJump);
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		}

		if (AttackAction)
		{
			EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &ALinearPlayerCharacter::Attack);
		}

		if (DefenseAction)
		{
			EnhancedInputComponent->BindAction(DefenseAction, ETriggerEvent::Started, this, &ALinearPlayerCharacter::StartDefense);
			EnhancedInputComponent->BindAction(DefenseAction, ETriggerEvent::Completed, this, &ALinearPlayerCharacter::StopDefense);
		}

		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ALinearPlayerCharacter::Interact);
		}

		if (RollingAction)
		{
			EnhancedInputComponent->BindAction(RollingAction, ETriggerEvent::Started, this, &ALinearPlayerCharacter::Rolling);
		}

		if (TargetAction)
		{
			EnhancedInputComponent->BindAction(TargetAction, ETriggerEvent::Started, this, &ALinearPlayerCharacter::Target);
		}

		if (AdvanceDialogueAction)
		{
			EnhancedInputComponent->BindAction(AdvanceDialogueAction, ETriggerEvent::Started, this, &ALinearPlayerCharacter::AdvanceDialogue);
		}
	}
}

void ALinearPlayerCharacter::Move(const FInputActionValue& Value)
{

	FVector2D MoveVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{

		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		// 回転行列
		const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // UEでいう正面
		const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); // UEでいう真右

		if (CanMove())
		{
			// ACharacter の持つ移動用関数
			AddMovementInput(ForwardDir, MoveVector.Y);
			AddMovementInput(RightDir, MoveVector.X);
		}
		else if (ActionState == EActionState::EAS_Attacking && bCanAttackSteering)
		{
			// 攻撃中移動は、移動入力の倍率を下げて微調整できるようにする
			// bOrientRotationToMovement が true なので入力方向へ振り向ける
			AddMovementInput(ForwardDir, MoveVector.Y * AttackSteeringMultiply);
			AddMovementInput(RightDir, MoveVector.X * AttackSteeringMultiply);
		}

	}
}

bool ALinearPlayerCharacter::CanMove()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void ALinearPlayerCharacter::Look(const FInputActionValue& Value)
{
	// ターゲットカメラ中は、マウス操作を切る
	if (PlayerTargeting && PlayerTargeting->IsLocked())
	{
		return;
	}

	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Controller (画面には描写されないもの)に回転を加える
		// Controller に追従するように傾く設定フラグなどがあるので、今回これに追従させたいので true としておくこと
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}


void ALinearPlayerCharacter::TryJump()
{
	// 何かジャンプ前にさせたいことがあれば、ここで書く
	//UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::TryJump()");
	if (ActionState == EActionState::EAS_Unoccupied)
	{
		Super::Jump();
	}
	else
	{
		//UE_LOGFMT(LogTemp, Warning, "Cannot exec Super::Jump(). Now Actioning.");
	}

}

void ALinearPlayerCharacter::Attack()
{
	//UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::Attack()");

	if (CanAttack())
	{
		PlayAttackMontage(); // 1団目の攻撃開始
		ActionState = EActionState::EAS_Attacking;
	}
	else if (ActionState == EActionState::EAS_Attacking && bCanSaveAttack)
	{
		bSaveAttack = true;
	}
	else
	{
		//UE_LOGFMT(LogTemp, Warning, "Cannot exec PlayAttackMontage(). Now Actioning or unequipped Weapon.");
	}

}

void ALinearPlayerCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{

		// ジャンプ攻撃
		if (GetCharacterMovement()->IsFalling() && JumpAttackMontage)
		{
			AnimInstance->Montage_Play(JumpAttackMontage, 1.0f);
		}

		else if (ComboMontages.Num() > 0 && ComboMontages[0] != nullptr)
		{
			// インデックスを0にリセットして1段目を再生
			ComboCountIndex = 0;
			AnimInstance->Montage_Play(ComboMontages[ComboCountIndex], 1.0f);
			ComboCountIndex++;
		}
	}
}

void ALinearPlayerCharacter::OnCanSaveAttack(bool bCanSave)
{
	bCanSaveAttack = bCanSave;
}

void ALinearPlayerCharacter::OnCheckCombo()
{
	// 先行入力が保存されていれば、次の攻撃セクションへ移行する
	if (bSaveAttack)
	{
		bSaveAttack = false;

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && ComboMontages.Num() > 0)
		{
			// コンボ上限に達していない場合のみ遷移
			if (ComboCountIndex < ComboMontages.Num())
			{
				// 次のMontageを再生（自動的に前のMontageからブレンドされる）
				AnimInstance->Montage_Play(ComboMontages[ComboCountIndex], 1.0f);
				ComboCountIndex++;
			}
		}
	}
}

void ALinearPlayerCharacter::OnAttackSteering(bool bCanSteer)
{
	bCanAttackSteering = bCanSteer;
}


void ALinearPlayerCharacter::OnAttackAnimEnded()
{
	ActionState = EActionState::EAS_Unoccupied;

	// コンボに関する状態をすべてリセット
	ComboCountIndex = 0;
	bCanSaveAttack = false;
	bSaveAttack = false;
}

bool ALinearPlayerCharacter::CanAttack()
{
	return
		ActionState == EActionState::EAS_Unoccupied &&
		EquippedWeapon != nullptr
		//CharacterState != ECharacterState::ECS_Unequipped
		;
}

// Defense は ABP State で管理する
void ALinearPlayerCharacter::StartDefense()
{
	if (CanDefense())
	{
		//UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::StopDefense()");
		ActionState = EActionState::EAS_Defensing;
	}
	else
	{
		//UE_LOGFMT(LogTemp, Warning, "Cannot exec StartDefense()");
	}
}

void ALinearPlayerCharacter::StopDefense()
{
	if (ActionState == EActionState::EAS_Defensing)
	{
		//UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::StopDefense()");
		ActionState = EActionState::EAS_Unoccupied;
	}
	else
	{
		//UE_LOGFMT(LogTemp, Warning, "Cannot exec StopDefense()");
	}
}

bool ALinearPlayerCharacter::CanDefense()
{
	return
		ActionState == EActionState::EAS_Unoccupied &&
		EquippedShield != nullptr
		;
}


void ALinearPlayerCharacter::Interact()
{
	// 1. 目の前に IInteractInterface を持った Actor があれば、会話開始
	if (OverlappingInteractableActor)
	{
		if (OverlappingInteractableActor->Implements<UInteractInterface>()) 
		{
			// 第一引数: 対象Actor 第二引数以降は定義した関数を渡す
			// Interact 時の処理自体は、BP_Prisoner の場合、BP 側だけで設計している
			// C++ で作ることもできるが、BP だけの設計にも慣れておこう
			IInteractInterface::Execute_Interact(OverlappingInteractableActor, this);

			// 会話状態
			ActionState = EActionState::EAS_InDialogue;

			// Dialogue の持つ、会話終了 Delegate に購読しておく
			ULinearDialogueComponent* DialogueComp = OverlappingInteractableActor->FindComponentByClass<ULinearDialogueComponent>();
			if (DialogueComp)
			{
				// 会話が終了したとき、EAS が 戻るような関数を紐づけておく
				DialogueComp->OnDialogueFinished.AddUniqueDynamic(this, &ALinearPlayerCharacter::OnDialogueEnd);
			}
		}
		return;
	}

	// 2. 装備処理
	// TODO: リファクタリングできる余地がある（Interface で Equipable など）
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	AShield* OverlappingShield = Cast<AShield>(OverlappingItem);

	if (OverlappingWeapon)
	{
		OverlappingWeapon->Equip(GetMesh(), RightHandSocketName, this, this);
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;// 暫定で全て片手武器
		EquippedWeapon = OverlappingWeapon;
		return;
	}
	else if (OverlappingShield)
	{
		OverlappingShield->Equip(GetMesh(), LeftHandSocketName, this, this);
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;// TODO: 盾だけのStateを作ってもよい
		EquippedShield = OverlappingShield;
		return;
	}
}

void ALinearPlayerCharacter::OnDialogueEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
	ActiveDialogueComponent = nullptr;

	// 会話中 IMC の解除
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(DialogueMappingContext);
		}
	}
}

void ALinearPlayerCharacter::OnWeaponCollisionEnabled(
	ECollisionEnabled::Type CollisionEnabled, float DamageMultiplier, float PoiseMultiplier
)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->BoxIgnoreActors.Empty();
		EquippedWeapon->SetMultipliers(DamageMultiplier, PoiseMultiplier);
	}
}

void ALinearPlayerCharacter::OnWeaponCollisionDisabled(ECollisionEnabled::Type CollisionDisabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionDisabled);
		EquippedWeapon->ResetMultipliers();
	}
}


void ALinearPlayerCharacter::Rolling()
{
	//UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::Rolling()");
	if (CanRolling())
	{
		ActionState = EActionState::EAS_Rolling;
		// Anim に設定されている進行量自体を増やす
		AnimRootMotionTranslationScale = 1.5f;
		PlayRollingMontage();
	}
	else
	{
		//UE_LOGFMT(LogTemp, Warning, "Cannot exec PlayRollingMontage(). Now Actioning.");
	}

}

void ALinearPlayerCharacter::PlayRollingMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	AnimInstance->Montage_Play(RollingMontage, 1.0f, EMontagePlayReturnType::MontageLength, .0f, true);
}


void ALinearPlayerCharacter::OnRollingAnimEnded()
{
	AnimRootMotionTranslationScale = 1.f;
	ActionState = EActionState::EAS_Unoccupied;
}

void ALinearPlayerCharacter::AttachRollingFieldSystem()
{
	UWorld* World = GetWorld();
	if (World)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this; // 生み出す FieldSystem の親を自身に設定
		FieldSystemActor = World->SpawnActor<AActor>(
			// 親と同じ Locate, Rotate などでスポーン
			FieldActorClass, GetActorLocation(), GetActorRotation(), SpawnParams
		);
		if (FieldSystemActor)
		{
			// LPCharacter にアタッチ
			FieldSystemActor->AttachToComponent(
				GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale
			);
		}
	}
}

void ALinearPlayerCharacter::OnRollingFieldNotifyBegin()
{
	AttachRollingFieldSystem();
}

void ALinearPlayerCharacter::DetachRollingFieldSystem()
{
	FieldSystemActor->Destroy();
	FieldSystemActor = nullptr;
}

void ALinearPlayerCharacter::OnRollingFieldNotifyEnd()
{
	DetachRollingFieldSystem();
}

bool ALinearPlayerCharacter::CanRolling()
{
	// TODO: 攻撃の後隙中にキャンセルさせたい
	return 
		ActionState == EActionState::EAS_Unoccupied &&
		! GetCharacterMovement()->IsFalling()
	;
}

// ダメージ処理(Interface より手前で呼ばれる)
float ALinearPlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// TODO: 防御中かどうかで処理を分ける（ダメージを抑えるかとか）

	if (Attributes)
	{
		Attributes->ReceiveHealthDamage(DamageAmount);
		//UE_LOGFMT(
		//	LogTemp, Warning, 
		//	"ALinearPlayerCharacter::TakeDamage() CurrentHealthPercent: {0}", Attributes->GetHealthPercent()
		//);
	}
	return DamageAmount;
}

// ダメージ後、状況に応じた処理
void ALinearPlayerCharacter::GetHit_Implementation(
	const FVector& ImpactPoint, const float FinalPoiseDamage
)
{
	//UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::GetHit_Implementation()");

	// TODO: 防御中かどうかで Anime を調整。外積などで前方180°なら... というようにしたい

	// アニメ再生 （やられ or 死亡）
	if (Attributes && Attributes->IsAlive())
	{
		// 生存 Poise に応じて 怯みアニメ再生
		const bool bIsStaggered = Attributes->IsStaggeredWithPoise(FinalPoiseDamage);
		if (bIsStaggered)
		{
			//UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter Poise Broken!");
			PlayHitReactionMontage(); // 怯みアニメの再生
			ActionState = EActionState::EAS_Hitting;
			Attributes->ResetPoise(); // 怯んだときは、Poise を最大にリセット
		}
		else
		{
			//UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter Poise Intaract: No reaction.");
		}
	}
	else
	{
		// 死亡 アニメ再生、State 変更
		Die();
	}

	// Overlay への HealthBar, PoiseBar 反映はデリゲートで対応済
	// Attribute の値が変わった時、Overlay に反映される設計に BeginPlay() でしている

	// HitSoundなど
 	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}
}

void ALinearPlayerCharacter::OnSaveGame(ULinearSaveGame* SaveGameObj)
{
	if (!SaveGameObj) return;

	// 自身のデータをSaveGameObj に格納する
	// (Player の情報は、Player が書き込むというような責務を持たせる）
	SaveGameObj->PlayerTransform = GetActorTransform();
	if (Attributes)
	{
		SaveGameObj->PlayerHealth = Attributes->GetCurrentHealth();
	}
	UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::OnSaveGame() Data extracted successfully.");
}

void ALinearPlayerCharacter::OnLoadGame(ULinearSaveGame* SaveGameObj)
{
	if (!SaveGameObj) return;

	// SaveGameObj の情報を自身に反映
	// (Player に関する情報は自分が取り出して、自分で設定）
	SetActorTransform(SaveGameObj->PlayerTransform);
	if (Attributes)
	{
		Attributes->SetCurrentHealth(SaveGameObj->PlayerHealth);
	}

	UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::OnLoadGame() Data applied successfully.");
}

void ALinearPlayerCharacter::PlayHitReactionMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactionMontage)
	{
		AnimInstance->Montage_Play(HitReactionMontage, 1.0f, EMontagePlayReturnType::MontageLength, .0f, true);
	}
}

void ALinearPlayerCharacter::OnHitReactionAnimEnded()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ALinearPlayerCharacter::Die()
{
	UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::Die()");

	PlayDeathMontage();
	ActionState = EActionState::EAS_Dying; // 死亡時は、この EAS から変わることはない

	// Collision 無効化 
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	// Tag を外す
	Tags.Remove(GetTag());

	// Enemy の CombatTarget を外す, カメラズーム（死亡シーンのカメラ画角にするなど）
	// そのあたりはデリゲートでやる
	OnCharacterDeathDelegate.Broadcast();

}

void ALinearPlayerCharacter::PlayDeathMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage && DeathMontageSectionNames.Num() > 0)
	{
		AnimInstance->Montage_Play(DeathMontage, 1.0f, EMontagePlayReturnType::MontageLength, .0f, true);

		const int32 Selection = FMath::RandRange(0, 1);
		FName SectionName = FName();
		switch (Selection)
		{
			case 0:
				SectionName = DeathMontageSectionNames[0];
				DeathPose = EDeathPose::EDP_Death1;
				break;
			case 1:
				SectionName = DeathMontageSectionNames[1];
				DeathPose = EDeathPose::EDP_Death2;
				break;
			default:
				break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}
}

void ALinearPlayerCharacter::Target()
{
	UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::Target()");
	if (PlayerTargeting)
	{
		PlayerTargeting->OnLockOnPressed();
	}
}

void ALinearPlayerCharacter::AdvanceDialogue()
{
	UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::AdvanceDialogue()");
	if (ActiveDialogueComponent)
	{
		ActiveDialogueComponent->AdvanceDialogue();
	}
}


// ===== Debug =====
//void ALinearPlayerCharacter::DebugSaveGame()
//{
//	if (UGameInstance* GI = GetGameInstance())
//	{
//		if (ULinearSaveSubsystem* SaveSubsystem = GI->GetSubsystem<ULinearSaveSubsystem>())
//		{
//			SaveSubsystem->SaveGame(this, 0);
//		}
//	}
//}
//
//void ALinearPlayerCharacter::DebugLoadGame()
//{
//	if (UGameInstance* GI = GetGameInstance())
//	{
//		if (ULinearSaveSubsystem* SaveSubsystem = GI->GetSubsystem<ULinearSaveSubsystem>())
//		{
//			SaveSubsystem->LoadGame(this, 0);
//		}
//	}
//}