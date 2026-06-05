#include "Enemies/EnemyBase.h"
#include "Logging/StructuredLog.h"
#include "LinearDungeon/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h" // DrawDebugAllow

// Component 関連
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// 音関連
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

// Navigation
#include "AIController.h"
#include "NavigationData.h"
#include "Navigation/PathFollowingComponent.h"
#include "AITypes.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
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

	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	if (SightConfig)
	{
		// 視覚パラメータの初期設定
		SightConfig->SightRadius = 1500.f;// 視認可能な半径
		SightConfig->LoseSightRadius = 1600.f; // 見失う半径（チラつき防止のため少し大きめに設定する）
		SightConfig->PeripheralVisionAngleDegrees = 45.f; // 視野角
		SightConfig->SetMaxAge(3.f);// 記憶保持時間

		// どの所属（敵、味方、中立）を検知するか
		// デフォルトでは全て検知しない設定のため、明示的にtrueにする
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

		// Perceptionコンポーネントに設定を登録
		AIPerceptionComponent->ConfigureSense(*SightConfig);
		AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
	}

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
	if (AIPerceptionComponent)
	{
		AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyBase::OnTargetDetected);
	}
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
	EnemyController = Cast<AAIController>(GetController());
	if (PatrolTarget == nullptr && PatrolTargets.Num() > 0)
	{
		UE_LOGFMT(LogTemp, Warning, "AEnemyBase::BeginPlay() Setting PatrolTarget = PatrolTargets[0]");
		PatrolTarget = PatrolTargets[0];
	}

	MoveToTarget(PatrolTarget);

}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 攻撃モーション中の特定期間(NotifyState) でフラグが制御されている間、相手に振り向く
	if (bIsTrackingTarget && CombatTarget != nullptr)
	{
		UpdateTrackingRotation(DeltaTime);
	}

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		// Chase / Attack 中に判断される処理
		CheckCombatTarget();
	}
	else
	{
		// Patrol 中に判断される処理
		CheckPatrolTarget();
	}
}

void AEnemyBase::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemyBase::PatrolTimerFinished, PatrolWaitingTime);
	}
}

void AEnemyBase::CheckCombatTarget()
{
	// 攻撃モーション中は距離による状態繊維をブロックする
	if (EnemyState == EEnemyState::EES_Attacking) return;

	// 検知範囲外になった時、HealthBar 非表示 / State を Patrol に戻して、元のターゲットに対象を戻す
	if (!InTargetRange(CombatTarget, CombatRadius))
	{
		CombatTarget = nullptr;
		if (HealthBarWidget)
		{
			HealthBarWidget->SetVisibility(false);
		}
		EnemyState = EEnemyState::EES_Patrolling;
		MoveToTarget(PatrolTarget);
		UE_LOGFMT(LogTemp, Log, "CheckCombatTarget(): Chase stop");

	}
	// 攻撃範囲外なら、攻撃が当たる範囲まで追いかける
	else if (!InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Chasing)
	{
		EnemyState = EEnemyState::EES_Chasing;
		MoveToTarget(CombatTarget);
		UE_LOGFMT(LogTemp, Log, "CheckCombatTarget(): Chasing");
	}
	// 攻撃範囲内 攻撃処理
	else if (InTargetRange(CombatTarget, AttackRadius) && EnemyState != EEnemyState::EES_Attacking)
	{
		EnemyState = EEnemyState::EES_Attacking;
		UE_LOGFMT(LogTemp, Log, "CheckCombatTarget(): Attack!");

		// 動きを止める
		if (EnemyController)
		{
			EnemyController->StopMovement();
			UE_LOGFMT(LogTemp, Log, "StopMovement()");
		}

		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(AttackMontage, 1.0f, EMontagePlayReturnType::MontageLength, .0f, true);
		AnimInstance->Montage_JumpToSection(FName("Attack1"), AttackMontage);
	}
}




// 発見したか、見失ったのかを判定
void AEnemyBase::OnTargetDetected(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if (EnemyState == EEnemyState::EES_Chasing) return;

		if (Actor->ActorHasTag(ALinearPlayerCharacter::GetTag()))
		{
			// チェイスして Character にたどり着いたときは、タイマーの待機処理は必要ないので無効にしておく
			GetWorldTimerManager().ClearTimer(PatrolTimer);

			if (EnemyState != EEnemyState::EES_Attacking)
			{
				EnemyState = EEnemyState::EES_Chasing;
				CombatTarget = Actor;
				MoveToTarget(CombatTarget);
				UE_LOGFMT(LogTemp, Log, "AEnemyBase::OnTargetDetected() Detected target: {0} Chasing!", Actor->GetName());
			}
		}
	}
	else
	{
		UE_LOGFMT(LogTemp, Log, "AEnemyBase::OnTargetDetected() Lost sight of target: {0}", Actor->GetName());
		// TODO: 見失ったあとの処理（数秒待機してPatrolに戻るなど）
		//EnemyState = EEnemyState::EES_Patrolling;
		//CheckPatrolTarget();

	}
}

bool AEnemyBase::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;

	// A から B のベクトル : B - A
	// 自身（敵） から 対象 へのベクトル計算
	const double DistanceToTarget =
		(Target->GetActorLocation() - GetActorLocation()).Size();

	DRAW_SPHERE_SingleFrame(GetActorLocation());
	DRAW_SPHERE_SingleFrame(Target->GetActorLocation())

	// 指定のゾーン（Radius) 内なら true, そうでない（範囲外）なら false
	return DistanceToTarget <= Radius;
}

void AEnemyBase::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;

	FAIMoveRequest MoveRequest; // AI に対する移動の要求内容をまとめた Struct
	MoveRequest.SetGoalActor(Target); // 目的地を特定の Actor に設定（Actor が動けば、併せて追跡するように再計算する）
	MoveRequest.SetAcceptanceRadius(30.f); // 目的地に着いたと許容する半径
	EnemyController->MoveTo(MoveRequest); 
}

AActor* AEnemyBase::ChoosePatrolTarget()
{
	// Target 切り替え時、既存の Target を再度対象に選ばないようにする
	TArray<AActor*> ValidTargets;
	for (auto Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	// ランダムに配列から Target を取得
	const int32 NumPatrolTargets = ValidTargets.Num();
	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}

	return nullptr;
}

void AEnemyBase::Die()
{
	UE_LOGFMT(LogTemp, Warning, "AEnemyBase::Die()");

	// 判定, HealthBar 非表示
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (HealthBarWidget)
	{
		UE_LOGFMT(LogTemp, Warning, "HealthBarWidget OFF");
		HealthBarWidget->SetVisibility(false);
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
	// TODO: 分岐させるべきか？
	if (CombatTarget != nullptr)
	{
		EnemyState = EEnemyState::EES_Chasing;
		MoveToTarget(CombatTarget); // Attack で StopMovement としたので、再開
	}
	else
	{
		EnemyState = EEnemyState::EES_Patrolling;
		MoveToTarget(PatrolTarget); // 一応再度実行しておく
	}
}

void AEnemyBase::OnTrackingTarget(bool bIsTracking)
{
	bIsTrackingTarget = bIsTracking;
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

	// 攻撃された相手
	if (EventInstigator != nullptr)
	{
		AActor* InstigatorPawn = EventInstigator->GetPawn();
		if (InstigatorPawn != nullptr)
		{
			CombatTarget = InstigatorPawn;
			EnemyState = EEnemyState::EES_Chasing;
			MoveToTarget(CombatTarget);
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

// Timer を設定し、指定時間後この関数が発火する
void AEnemyBase::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);

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

