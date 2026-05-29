#include "Characters/LinearPlayerCharacter.h"
#include "Logging/StructuredLog.h"

#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/ItemBase.h"
#include "Items/Weapon.h"
#include "Items/Shield.h"
#include "Animation/AnimMontage.h"
#include "Components/BoxComponent.h"

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

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmBlankCharacter"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 400.f;
	SpringArm->bUsePawnControlRotation = true; // Controler の回転を SpringArmに反映させる
	SpringArm->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

void ALinearPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
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
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
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
			SectionName = AttackMontageSectionNames[2];
		}
		else
		{
			const int32 Selection = FMath::RandRange(0, 1);
			switch (Selection)
			{
			case 0:
				SectionName = AttackMontageSectionNames[0];
				UE_LOGFMT(LogTemp, Warning, "0 {Selection}", Selection);

				break;
			case 1:
				SectionName = AttackMontageSectionNames[1];
				UE_LOGFMT(LogTemp, Warning, "1 {Selection}", Selection);

				break;
			default:
				UE_LOGFMT(LogTemp, Warning, "default");
				break;
			}
		}

		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);

	}
}

// Anim Montage の Notifies, ABP などから呼ぶ
void ALinearPlayerCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
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

	//AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
	//if (OverlappingWeapon)
	//{
	//	OverlappingWeapon->Equip(GetMesh(), RightHandSocketName, this, this);
	//	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;// TODO: 暫定で全て片手武器
	//	EquippedWeapon = OverlappingWeapon;
	//}

}

void ALinearPlayerCharacter::Rolling()
{
	UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::Rolling()");
	if (CanRolling())
	{
		PlayRollingMontage();
		ActionState = EActionState::EAS_Rolling;
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


void ALinearPlayerCharacter::RollingEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}


bool ALinearPlayerCharacter::CanRolling()
{
	// どんな時でもキャンセルできると面白いかもしれない
	return ActionState == EActionState::EAS_Unoccupied;
}

void ALinearPlayerCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->BoxIgnoreActors.Empty();
	}
}

void ALinearPlayerCharacter::SetWeaponCollisionDisabled(ECollisionEnabled::Type CollisionDisabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionDisabled);
	}
}
