#include "Enemies/EnemyBase.h"
#include "Logging/StructuredLog.h"
#include "LinearDungeon/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h" // DrawDebugAllow

// Component 関連
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Controllers/LinearEnemyAIController.h"

// 音関連
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

// Navigation
//#include "AIController.h"
//#include "NavigationData.h"
//#include "Navigation/PathFollowingComponent.h"
//#include "AITypes.h"
//#include "Perception/AIPerceptionComponent.h"
//#include "Perception/AISenseConfig_Sight.h"
#include "Characters/LinearPlayerCharacter.h"

// 攻撃判定
#include "Components/BoxComponent.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// WeaponBox の判定と感知するように, また カメラに干渉しないように設定
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	// Components 追加
	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	// AIController クラスを自作したものに置き換える
	AIControllerClass = ALinearEnemyAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	// 敵が方向転換するとき、かくかくした Animation にならないようにする
	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;

	// ===== 攻撃判定設定 =====
	RightHandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightHandBox"));
	RightHandCollision->SetupAttachment(GetMesh(), FName("hand_r")); // Bone に割り当てる
	RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightHandCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	// Player(Pawn) に対してのみ動作させる
	RightHandCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightHandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	RightHandCollision->SetGenerateOverlapEvents(true);

	LeftHandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandBox"));
	LeftHandCollision->SetupAttachment(GetMesh(), FName("hand_l"));
	LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftHandCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftHandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	LeftHandCollision->SetGenerateOverlapEvents(true);

}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	// デリゲート登録
	if (RightHandCollision)
	{
		RightHandCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnRightHandOverlap);
	}
	if (LeftHandCollision)
	{
		LeftHandCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnLeftHandOverlap);
	}


	// 初期は、HP bar は消しておく（被弾したら表示する）
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}

	// PatrolTarget を初期決定していなければ、[0]を初期対象とする
	//EnemyController = Cast<AAIController>(GetController());
	if (PatrolTarget == nullptr && PatrolTargets.Num() > 0)
	{
		UE_LOGFMT(LogTemp, Warning, "AEnemyBase::BeginPlay() Setting PatrolTarget = PatrolTargets[0]");
		PatrolTarget = PatrolTargets[0];
	}

}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 攻撃モーション中の特定期間(NotifyState) でフラグが制御されている間、相手に振り向く
	if (bIsTrackingTarget && CombatTarget != nullptr)
	{
		UpdateTrackingRotation(DeltaTime);
	}
}

void AEnemyBase::Die()
{
	UE_LOGFMT(LogTemp, Warning, "AEnemyBase::Die()");

	// 振り向きの終了及び、移動の完成を止めて無効化
	bIsTrackingTarget = false;
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	// 判定, HealthBar 非表示
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (HealthBarWidget)
	{
		UE_LOGFMT(LogTemp, Warning, "HealthBarWidget OFF");
		HealthBarWidget->SetVisibility(false);
	}

	// AI Controller 側の 憑依解除
	if (ALinearEnemyAIController* AIController = Cast<ALinearEnemyAIController>(GetController()))
	{
		AIController->HandleEnemyDeath();
	}

	// 一定時間経過したら、Destroy されるようにする
	SetLifeSpan(5.f);

	// Montage 再生
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage, 1.0f, EMontagePlayReturnType::MontageLength, .0f, true);

		const int32 Selection = FMath::RandRange(0, 2);
		FName SectionName = FName();
		switch (Selection)
		{
		case 0:
			SectionName = FName("Death1");
			DeathPose = EDeathPose::EDP_Death1;
			break;
		case 1:
			SectionName = FName("Death2");
			DeathPose = EDeathPose::EDP_Death2;
			break;
		case 2:
			SectionName = FName("Death3");
			DeathPose = EDeathPose::EDP_Death3;
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);

		UE_LOGFMT(LogTemp, Warning, "Playing DeathMontage {Name}", SectionName);

	}

}

void AEnemyBase::OnRightHandOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult
)
{
	if (OtherActor && OtherActor != this && OtherActor->ActorHasTag(ALinearPlayerCharacter::GetTag()))
	{
		UE_LOGFMT(LogTemp, Warning, "AEnemyBase::OnRightHandOverlap");
		// とりあえず固定で 10.0 のダメージ
		UGameplayStatics::ApplyDamage(
			OtherActor, 10.f, GetController(), this, UDamageType::StaticClass()
		);

		// Interface に応じた固有処理
		IHitInterface* HitInterface = Cast<IHitInterface>(OtherActor);
		if (HitInterface)
		{
			HitInterface->Execute_GetHit(OtherActor, OtherActor->GetActorLocation());
		}
	}
}

// TODO: 両手攻撃でどちらも処理が走ってしまうので、調整対応する
void AEnemyBase::OnLeftHandOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult
)
{
	if (OtherActor && OtherActor != this &&	OtherActor->ActorHasTag(ALinearPlayerCharacter::GetTag()))
	{
		UE_LOGFMT(LogTemp, Warning, "AEnemyBase::OnLeftHandOverlap");
		UGameplayStatics::ApplyDamage(
			OtherActor, 10.f, GetController(), this, UDamageType::StaticClass()
		);

		// Interface に応じた固有処理
		IHitInterface* HitInterface = Cast<IHitInterface>(OtherActor);
		if (HitInterface)
		{
			HitInterface->Execute_GetHit(OtherActor, OtherActor->GetActorLocation());
		}
	}
}

void AEnemyBase::ActivateAttackCollision(EAttackCollisionType CollisionType)
{
	switch (CollisionType)
	{
		case EAttackCollisionType::EAC_RightHand:
			if (RightHandCollision)
			{
				RightHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
			break;
		case EAttackCollisionType::EAC_LeftHand:
			if (RightHandCollision)
			{
				RightHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
			break;
		case EAttackCollisionType::EAC_BothHands:
			if (RightHandCollision)
			{
				RightHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
			if (RightHandCollision)
			{
				RightHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
			break;
	}
}

void AEnemyBase::OnAttackCollisionNotifyBegin(EAttackCollisionType CollisionType)
{
	ActivateAttackCollision(CollisionType);
}

void AEnemyBase::DeactivateAttackCollision()
{
	if (RightHandCollision)
	{
		RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (LeftHandCollision)
	{
		LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AEnemyBase::OnAttackCollisionNotifyEnd()
{
	DeactivateAttackCollision();
}

void AEnemyBase::OnAttackEnd()
{
	// TODO: 
	// 移動の再開処理は、Behavior Tree にすべて持たせる
	// なのでこの Anim Notify は不要になったので、後で削除しておこう

	//if (CombatTarget != nullptr)
	//{
	//	EnemyState = EEnemyState::EES_Chasing;
	//	MoveToTarget(CombatTarget); // Attack で StopMovement としたので、再開
	//}
	//else
	//{
	//	EnemyState = EEnemyState::EES_Patrolling;
	//	MoveToTarget(PatrolTarget); // 一応再度実行しておく
	//}
}

void AEnemyBase::OnTrackingTarget(bool bIsTracking)
{
	bIsTrackingTarget = bIsTracking;
}

AActor* AEnemyBase::OnGetNextPatrolTarget()
{
	// 現在のターゲット以外の Patrol 対象配列作成
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	// 現時点のもの以外のデータを格納した配列から選ぶことで、同じポイントを選ばないようにする

	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		PatrolTarget = ValidTargets[TargetSelection];
		return PatrolTarget;
	}

	return nullptr;
}

void AEnemyBase::OnPerformAttack()
{
	UE_LOGFMT(LogTemp, Log, "AEnemyBase::PerformAttack() Attack!");

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage, 1.0f, EMontagePlayReturnType::MontageLength, .0f, true);
		AnimInstance->Montage_JumpToSection(FName("Attack1"), AttackMontage);
	}
}


void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::GetHit_Implementation(const FVector& ImpactPoint)
{
	UE_LOGFMT(LogTemp, Warning, "AEnemyBase::GetHit_Implementation()");
	//DRAW_SPHERE_COLOR(ImpactPoint, FColor::Orange);

	// HealthBar 表示
	if (HealthBarWidget && ! HealthBarWidget->IsVisible())
	{
		UE_LOGFMT(LogTemp, Warning, "HealthBarWidget ON");
		HealthBarWidget->SetVisibility(true);
	}

	// アニメ再生
	if (Attributes && Attributes->IsAlive())
	{
		// 生存 のけぞりアニメ再生
		UE_LOGFMT(LogTemp, Warning, "DirectionalHitReact");
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		// 死亡処理, アニメ再生
		UE_LOGFMT(LogTemp, Warning, "Die");
		Die();
	}

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}

	if (HitParticle && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), HitParticle, ImpactPoint
		);
	}

}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// Widget 更新処理
	if (Attributes && HealthBarWidget)
	{
		Attributes->ReceiveDamage(DamageAmount);
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}

	// 攻撃された相手を対象にする処理
	if (EventInstigator != nullptr)
	{
		AActor* InstigatorPawn = EventInstigator->GetPawn();
		if (InstigatorPawn != nullptr)
		{
			// TODO : Blackboard 側の CombatTarget に書き込む処理にする


			//CombatTarget = InstigatorPawn;
			//EnemyState = EEnemyState::EES_Chasing;
			//MoveToTarget(CombatTarget);
		}
	}

	return DamageAmount;
}

void AEnemyBase::DirectionalHitReact(const FVector& ImpactPoint)
{
	// 当たった角度に応じて再生する Montage を決定する
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// 角度を内積を用いて導く
	// Forward * ToHit = |Forward| |ToHit| * cosθ
	// 正規化しているので、実質 = cosθ である
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheta); // cosθ から、θ(角度)だけを逆三角関数を用いて取り出す
	Theta = FMath::RadiansToDegrees(Theta);

	// 導いた角度が + か - かを、外積で出す
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	//UE_LOGFMT(LogTemp, Warning, "%f", Theta); エラーになる
	/*
		UE_LOGFMT(LogTemp, Warning, "{0}", Theta);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("%f"), Theta));
		}


		UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);
		UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.f);
		UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 60.f, 5.f, FColor::Blue, 5.f);
	*/

	// 角度に応じて再生する Montage を決める
	// ↓ を正面としたとき、←が + →が - の角度

	FName Section("FromBack");
	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}

	PlayHitReactionMontage(Section);
}

// 攻撃モーション中に、対象方向にフラグが有効の間振り向かせる
void AEnemyBase::UpdateTrackingRotation(float DeltaTime)
{
	if (CombatTarget == nullptr) return;

	FVector StartLocation = GetActorLocation();
	FVector TargetLocation = CombatTarget->GetActorLocation();

	// Z 軸は無視
	TargetLocation.Z = StartLocation.Z;

	// ターゲットの方向を向くための理想の角度を計算して、滑らかに振り向かせる
	FRotator TargetRotation = (TargetLocation - StartLocation).Rotation();
	FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, TrackingInterpSpeed);
	SetActorRotation(InterpRotation);
}

void AEnemyBase::PlayHitReactionMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactionMontage)
	{
		AnimInstance->Montage_Play(HitReactionMontage, 1.0f, EMontagePlayReturnType::MontageLength, .0f, true);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactionMontage);
	}

}

