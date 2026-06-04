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

// 扱う Actor
#include "Items/ItemBase.h"
#include "Items/Weapon.h"
#include "Items/Shield.h"
#include "Animation/AnimMontage.h"

// 音関連
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

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
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bUsePawnControlRotation = true; // Controler の回転を SpringArmに反映させる
	SpringArm->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

}

void ALinearPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	Tags.Add(FName("LinearPlayerCharacter"));
}

void ALinearPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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

		if (EquipAction)
		{
			EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &ALinearPlayerCharacter::Equip);
		}

		if (RollingAction)
		{
			EnhancedInputComponent->BindAction(RollingAction, ETriggerEvent::Started, this, &ALinearPlayerCharacter::Rolling);

		}

	}
}

void ALinearPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (! CanMove()) return;

	FVector2D MoveVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{

		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		// 回転行列
		const FVector ForwardDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); // UEでいう正面
		const FVector RightDir = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); // UEでいう真右

		// ACharacter の持つ移動用関数
		AddMovementInput(ForwardDir, MoveVector.Y);
		AddMovementInput(RightDir, MoveVector.X);
	}
}

bool ALinearPlayerCharacter::CanMove()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void ALinearPlayerCharacter::Look(const FInputActionValue& Value)
{
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
	UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::TryJump()");
	if (ActionState == EActionState::EAS_Unoccupied)
	{
		Super::Jump();
	}
	else
	{
		UE_LOGFMT(LogTemp, Warning, "Cannot exec Super::Jump(). Now Actioning.");
	}

}

void ALinearPlayerCharacter::Attack()
{
	UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::Attack()");

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
		UE_LOGFMT(LogTemp, Warning, "Cannot exec PlayAttackMontage(). Now Actioning or unequipped Weapon.");
	}

}

void ALinearPlayerCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage && AttackMontageSectionNames.Num() > 0)
	{
		AnimInstance->Montage_Play(AttackMontage, 1.0f, EMontagePlayReturnType::MontageLength, .0f, true);
		FName SectionName = FName();

		// ジャンプ中ならジャンプ攻撃、そうでないなら通常攻撃
		if (GetCharacterMovement()->IsFalling())
		{
			SectionName = AttackMontageSectionNames[4];
			AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
		}
		else
		{
			SectionName = AttackMontageSectionNames[ComboCountIndex];
			AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);

			// 2回目以降は、OnCheckCombo() で操作する
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
		if (AnimInstance && AttackMontage && AttackMontageSectionNames.Num() > 0)
		{
			// コンボ上限に達していない場合のみ遷移
			if (ComboCountIndex <= MaxComboIndex)
			{
				FName NextSection = AttackMontageSectionNames[ComboCountIndex];
				AnimInstance->Montage_JumpToSection(NextSection, AttackMontage);
				ComboCountIndex++;
			}
		}
	}
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
		UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::StopDefense()");
		ActionState = EActionState::EAS_Defensing;
	}
	else
	{
		UE_LOGFMT(LogTemp, Warning, "Cannot exec StartDefense()");
	}
}

void ALinearPlayerCharacter::StopDefense()
{
	if (ActionState == EActionState::EAS_Defensing)
	{
		UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::StopDefense()");
		ActionState = EActionState::EAS_Unoccupied;
	}
	else
	{
		UE_LOGFMT(LogTemp, Warning, "Cannot exec StopDefense()");
	}
}

bool ALinearPlayerCharacter::CanDefense()
{
	return
		ActionState == EActionState::EAS_Unoccupied &&
		EquippedShield != nullptr
	;
}


void ALinearPlayerCharacter::Equip()
{
	UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::Equip()");

	// TODO: リファクタリングできる余地がある（Interface で Equipable など）
	AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	AShield* OverlappingShield = Cast<AShield>(OverlappingItem);

	if (OverlappingWeapon)
	{
		OverlappingWeapon->Equip(GetMesh(), RightHandSocketName, this, this);
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;// 暫定で全て片手武器
		EquippedWeapon = OverlappingWeapon;
	}
	else if (OverlappingShield)
	{
		OverlappingShield->Equip(GetMesh(), LeftHandSocketName, this, this);
		CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;// TODO: 盾だけのStateを作る
		EquippedShield = OverlappingShield;
	}
}

void ALinearPlayerCharacter::OnWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->BoxIgnoreActors.Empty();
	}
}

void ALinearPlayerCharacter::OnWeaponCollisionDisabled(ECollisionEnabled::Type CollisionDisabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionDisabled);
	}
}


void ALinearPlayerCharacter::Rolling()
{
	UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::Rolling()");
	if (CanRolling())
	{
		ActionState = EActionState::EAS_Rolling;
		// Anim に設定されている進行量自体を増やす
		AnimRootMotionTranslationScale = 1.5f;
		PlayRollingMontage();
	}
	else
	{
		UE_LOGFMT(LogTemp, Warning, "Cannot exec PlayRollingMontage(). Now Actioning.");
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
		Attributes->ReceiveDamage(DamageAmount);
		UE_LOGFMT(
			LogTemp, Warning, 
			"ALinearPlayerCharacter::TakeDamage() CurrentHealthPercent: {0}", Attributes->GetHealthPercent()
		);
	}
	return DamageAmount;
}

// ダメージ後、状況に応じた処理
void ALinearPlayerCharacter::GetHit_Implementation(const FVector& ImpactPoint)
{
	UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::GetHit_Implementation()");
	// TODO: 防御中かどうかで Anime を調整

	// HealthBar 反映

	// アニメ再生 （やられ or 死亡）
	if (Attributes && Attributes->IsAlive())
	{
		// 生存 のけぞりアニメ再生
		PlayHitReactionMontage();
		ActionState = EActionState::EAS_Hitting;
	}
	else
	{
		// 死亡 アニメ再生、State 変更
		Die();
	}

	// HitSoundなど
 	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation());
	}

	// State を作って、他の行動が食らいでキャンセルされないようにする
	// （State が切り替わらず、キャラが止まってしまうので）

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

	// Collision とかの無効化、Enemy の Target を外すなど

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
