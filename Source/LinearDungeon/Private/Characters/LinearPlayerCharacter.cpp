#include "Characters/LinearPlayerCharacter.h"
#include "Logging/StructuredLog.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"

ALinearPlayerCharacter::ALinearPlayerCharacter()
{
	// Controller の回転に Character 自身は依存しないことを明示する
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// 進行した方向にキャラクターが振り向く設定
	GetCharacterMovement()->bOrientRotationToMovement = true; // TODO: nullptr 等でチェックは不要か？
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

		// ACharacter の持つ移動用関数
		AddMovementInput(ForwardDir, MoveVector.Y);
		AddMovementInput(RightDir, MoveVector.X);
	}
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

	Super::Jump();
}

void ALinearPlayerCharacter::Attack()
{
	UE_LOGFMT(LogTemp, Warning, "ALinearPlayerCharacter::Attack()");
}

