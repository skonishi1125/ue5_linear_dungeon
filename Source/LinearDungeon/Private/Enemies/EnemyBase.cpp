#include "Enemies/EnemyBase.h"
#include "Logging/StructuredLog.h"
#include "LinearDungeon/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h" // DrawDebugAllow

// Component 関連
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/OverheadStatusWidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Controllers/LinearEnemyAIController.h"
#include "BehaviorTree/BlackboardComponent.h" // BB に CombatTarget を入れるために必要

// 汎用系
#include "Characters/LinearPlayerCharacter.h"

// 音関連
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

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
	OverheadStatusWidgetComponent = CreateDefaultSubobject<UOverheadStatusWidgetComponent>(TEXT("OverheadStatusWidget"));
	OverheadStatusWidgetComponent->SetupAttachment(GetRootComponent());

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
	// Player(Pawn) に対してのみ動作させるような形で設計
	RightHandCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightHandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	RightHandCollision->SetGenerateOverlapEvents(true);

	RightLegCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("RightLegBox"));
	RightLegCollision->SetupAttachment(GetMesh(), FName("calf_r"));
	RightLegCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RightLegCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	RightLegCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	RightLegCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	RightLegCollision->SetGenerateOverlapEvents(true);

	LeftHandCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftHandBox"));
	LeftHandCollision->SetupAttachment(GetMesh(), FName("hand_l"));
	LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftHandCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftHandCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftHandCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	LeftHandCollision->SetGenerateOverlapEvents(true);

	LeftLegCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("LeftLegBox"));
	LeftLegCollision->SetupAttachment(GetMesh(), FName("calf_l"));
	LeftLegCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LeftLegCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	LeftLegCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	LeftLegCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	LeftLegCollision->SetGenerateOverlapEvents(true);

	// 左右手足の攻撃始点を決める
	RightHandBoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Right Hand Box Trace Start"));
	RightHandBoxTraceStart->SetupAttachment(GetMesh(), FName("hand_r"));
	RightHandBoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Right Hand Box Trace End"));
	RightHandBoxTraceEnd->SetupAttachment(GetMesh(), FName("hand_r"));

	RightLegBoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Right Leg Box Trace Start"));
	RightLegBoxTraceStart->SetupAttachment(GetMesh(), FName("calf_r"));
	RightLegBoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Right Leg Box Trace End"));
	RightLegBoxTraceEnd->SetupAttachment(GetMesh(), FName("calf_r"));

	LeftHandBoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Left Hand Box Trace Start"));
	LeftHandBoxTraceStart->SetupAttachment(GetMesh(), FName("hand_l"));
	LeftHandBoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Left Hand Box Trace End"));
	LeftHandBoxTraceEnd->SetupAttachment(GetMesh(), FName("hand_l"));

	LeftLegBoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Left Leg Box Trace Start"));
	LeftLegBoxTraceStart->SetupAttachment(GetMesh(), FName("calf_l"));
	LeftLegBoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Left Leg Box Trace End"));
	LeftLegBoxTraceEnd->SetupAttachment(GetMesh(), FName("calf_l"));

}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	// デリゲート登録
	if (RightHandCollision)
	{
		RightHandCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnRightHandOverlap);
	}
	if (RightLegCollision)
	{
		RightLegCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnRightLegOverlap);
	}
	if (LeftHandCollision)
	{
		LeftHandCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnLeftHandOverlap);
	}
	if (LeftLegCollision)
	{
		LeftLegCollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemyBase::OnLeftLegOverlap);
	}


	// 初期は、OverheadStatusWidget は消しておく（被弾したら表示する）
	if (OverheadStatusWidgetComponent)
	{
		OverheadStatusWidgetComponent->SetVisibility(false);
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
	if (bIsTrackingTarget)
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
	// TODO: Weapon と Overlap が検知しないようにする
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (OverheadStatusWidgetComponent)
	{
		UE_LOGFMT(LogTemp, Warning, "OverheadStatusWidgetComponent OFF");
		OverheadStatusWidgetComponent->SetVisibility(false);
	}

	// AI Controller 側の 憑依解除
	if (CachedAIController)
	{
		CachedAIController->HandleEnemyDeath();
	}

	// 一定時間経過したら、Destroy されるようにする
	SetLifeSpan(5.f);

	// Montage 再生
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage, 1.0f, EMontagePlayReturnType::MontageLength, .0f, true);

		const int32 Selection = FMath::RandRange(0, 1);
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
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);

		//UE_LOGFMT(LogTemp, Warning, "Playing DeathMontage {Name}", SectionName);

	}

}

void AEnemyBase::OnRightHandOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult
)
{
	PerformAttackTrace(RightHandBoxTraceStart, RightHandBoxTraceEnd, TEXT("RightHand"));
}

void AEnemyBase::OnRightLegOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult
)
{
	PerformAttackTrace(RightLegBoxTraceStart, RightLegBoxTraceEnd, TEXT("RightLeg"));
}

void AEnemyBase::OnLeftHandOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult
)
{
	PerformAttackTrace(LeftHandBoxTraceStart, LeftHandBoxTraceEnd, TEXT("LeftHand"));
}

void AEnemyBase::OnLeftLegOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult
)
{
PerformAttackTrace(LeftLegBoxTraceStart, LeftLegBoxTraceEnd, TEXT("LeftLeg"));
}

// BoxTrace -> Damage 共通処理
void AEnemyBase::PerformAttackTrace(USceneComponent* TraceStart, USceneComponent* TraceEnd, const FString& DebugSocketName)
{
	if (!TraceStart || !TraceEnd) return;

	const FVector Start = TraceStart->GetComponentLocation();
	const FVector End = TraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	for (AActor* Actor : BoxIgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	FHitResult BoxHit;
	UKismetSystemLibrary::BoxTraceSingle(
		this, 
		Start, 
		End, 
		FVector(5.f, 5.f, 5.f),
		TraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore, 
		EDrawDebugTrace::ForDuration, 
		BoxHit, 
		true
	);

	AActor* HitActor = BoxHit.GetActor();
	if (HitActor)
		UE_LOGFMT(LogTemp, Warning, "{0}::Overlap! {1}", DebugSocketName, HitActor->GetName());
	else
		UE_LOGFMT(LogTemp, Warning, "{0}::No Hit", DebugSocketName);

	if (HitActor && HitActor != this && HitActor->ActorHasTag(ALinearPlayerCharacter::GetTag()))
	{
		const float FinalDamage = BaseDamage * CurrentDamageMultiplier;
		const float FinalPoiseDamage = BasePoiseDamage * CurrentPoiseMultiplier;

		UE_LOGFMT(
			LogTemp, Warning,
			"AEnemyBase::PerformAttackTrace() finalDmg: {0} finalPoise: {1}", FinalDamage, FinalPoiseDamage
		);

		UGameplayStatics::ApplyDamage(HitActor, FinalDamage, GetInstigator()->GetController(), this, UDamageType::StaticClass());

		IHitInterface* HitInterface = Cast<IHitInterface>(HitActor);
		if (HitInterface)
		{
			HitInterface->Execute_GetHit(HitActor, BoxHit.ImpactPoint, FinalPoiseDamage);
		}

		BoxIgnoreActors.AddUnique(HitActor);
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
			if (LeftHandCollision)
			{
				LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
			break;
		case EAttackCollisionType::EAC_RightLeg:
			if (RightLegCollision)
			{
				RightLegCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
			break;
		case EAttackCollisionType::EAC_LeftLeg:
			if (LeftLegCollision)
			{
				LeftLegCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
			break;
		case EAttackCollisionType::EAC_BothHands:
			if (RightHandCollision)
			{
				RightHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
			if (LeftHandCollision)
			{
				LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
			break;
		case EAttackCollisionType::EAC_BothLegs:
			if (RightLegCollision)
			{
				RightLegCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
			if (LeftLegCollision)
			{
				LeftLegCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			}
			break;
	}
}

void AEnemyBase::OnAttackCollisionNotifyBegin(
	EAttackCollisionType CollisionType,
	float DamageMultiplier, float PoiseMultiplier
)
{
	CurrentDamageMultiplier = CurrentDamageMultiplier * DamageMultiplier;
	CurrentPoiseMultiplier = CurrentPoiseMultiplier * PoiseMultiplier;

	ActivateAttackCollision(CollisionType);
}

void AEnemyBase::DeactivateAttackCollision()
{
	if (RightHandCollision)
	{
		RightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (RightLegCollision)
	{
		RightLegCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (LeftHandCollision)
	{
		LeftHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (LeftLegCollision)
	{
		LeftLegCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

// Romero AN_AttackCollision の Notify 終了時に呼ばれる処理
void AEnemyBase::OnAttackCollisionNotifyEnd()
{
	DeactivateAttackCollision();
	CurrentDamageMultiplier = 1.0f;
	CurrentPoiseMultiplier = 1.0f;
	BoxIgnoreActors.Empty(); // 重複して攻撃しない為に作った配列のリセット
	UE_LOGFMT(LogTemp, Warning, "AEnemyBase::OnAttackCollisionNotifyEnd() Reset! {0}", BoxIgnoreActors.Num());
	//UE_LOGFMT(LogTemp, Warning, "AEnemyBase::OnAttackCollisionNotifyEnd()");
}

// Delegate に登録されたタスクを実行
// 例えば、BT に Attack の挙動が正常に終了したことなどを伝えて、攻撃終了後どうするか、みたいな処理を決める
void AEnemyBase::OnAttackEnd()
{
	OnAttackEndDelegate.Broadcast();
}

void AEnemyBase::OnTrackingTarget(bool bIsTracking)
{
	bIsTrackingTarget = bIsTracking;
	UE_LOGFMT(LogTemp, Warning, "AEnemyBase::OnTrackingTarget: {0}", bIsTracking);

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
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AttackMontages.Num() > 0)
	{
		int32 MaxRange = AttackMontages.Num() - 1;
		const int32 AttackIndex = FMath::RandRange(0, MaxRange);

		if (AttackMontages[AttackIndex] != nullptr)
		{
			AnimInstance->Montage_Play(AttackMontages[AttackIndex], 1.0f);
		}
	}
}


void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Controller が憑依したときに一度だけ呼ばれる関数
	CachedAIController = Cast<ALinearEnemyAIController>(NewController);

	if (CachedAIController)
	{
		UE_LOGFMT(LogTemp, Warning, "AEnemyBase::PossessedBy() Successfully cached ALE_AIController.");
	}
	else
	{
		UE_LOGFMT(LogTemp, Warning, "AEnemyBase::PossessedBy() Failed cached ALE_AIController.");
	}

	// このタイミングではまだ、AIController は RunBehaviorTree を実行していない
	// なので、BB のキャッシュ対応などは一旦しない
}

void AEnemyBase::GetHit_Implementation(
	const FVector& ImpactPoint, const float FinalPoiseDamage
)
{
	UE_LOGFMT(LogTemp, Warning, "AEnemyBase::GetHit_Implementation()");
	UE_LOGFMT(LogTemp, Warning, "FinalPoiseDamage: {0}", FinalPoiseDamage);


	// OverheadStatusWidgetComponent 表示
	if (OverheadStatusWidgetComponent && !OverheadStatusWidgetComponent->IsVisible())
	{
		//UE_LOGFMT(LogTemp, Warning, "HealthBarWidget ON");
		OverheadStatusWidgetComponent->SetVisibility(true);
	}

	// アニメ再生
	if (Attributes && Attributes->IsAlive())
	{
		// ポイズ値を差し引きして、怯みチェック
		float ExcessPoiseDamage = 0.f; // 現状 EnemyBase では HardHit のケースを未実装のため、使っていない
		const bool bIsStaggered = Attributes->IsStaggeredWithPoise(FinalPoiseDamage, ExcessPoiseDamage);
		if (bIsStaggered)
		{
			UE_LOGFMT(LogTemp, Warning, "AEnemyBase::GetHit_Implementation Poise Broken!");

			// BB IsStaggered フラグを有効化して、怯み中に BT の動きを止める
			// このフラグの無効化自体は、怯みアニメに Notify を仕込んで操作するようにする
			if (CachedAIController)
			{
				if (UBlackboardComponent* BB = CachedAIController->GetBlackboardComponent())
				{
					BB->SetValueAsBool(FName("IsStaggered"), true);
				}
			}

			// 怯みアニメの再生
			DirectionalHitReact(ImpactPoint);

			// 攻撃中などに怯んだ場合、タスクが終わらなくなるので AttackEnd Delegate も実行しておく
			OnAttackEndDelegate.Broadcast();
			Attributes->ResetPoise();// ポイズ値を最大にリセット
		}
		else
		{
			// ポイズが残っている場合は怯まない（スーパーアーマー状態）
			UE_LOGFMT(LogTemp, Warning, "AEnemyBase::GetHit_Implementation Poise intact(No reaction)");
		}
	}
	else
	{
		// 死亡処理, アニメ再生
		UE_LOGFMT(LogTemp, Warning, "Die");
		Die();
	}

	// 怯んだときの OverheadWidget PoiseBar は、Attributes が自動で反映する
	//if (OverheadStatusWidgetComponent)
	//	OverheadStatusWidgetComponent->SetPoisePercent(Attributes->GetPoisePercent());

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

// 怯みフラグの無効化 Anim Notify 経由で使う
void AEnemyBase::OnStaggerEnd()
{
	if (CachedAIController)
	{
		if (UBlackboardComponent* BB = CachedAIController->GetBlackboardComponent())
		{
			BB->SetValueAsBool(FName("IsStaggered"), false);
		}
	}
}

float AEnemyBase::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// Attributes 更新
	// OverheadWidget は、Attributes が自動で反映する
	if (Attributes)
	{
		Attributes->ReceiveHealthDamage(DamageAmount);
		//OverheadStatusWidgetComponent->SetHealthPercent(Attributes->GetHealthPercent());
	}

	// 攻撃された相手を対象にする処理
	if (EventInstigator != nullptr)
	{
		AActor* InstigatorPawn = EventInstigator->GetPawn();
		if (InstigatorPawn != nullptr)
		{
			// Blackboard 側の CombatTarget に書き込む
			if (CachedAIController)
			{
				if (UBlackboardComponent* BB = CachedAIController->GetBlackboardComponent())
				{
					BB->SetValueAsObject(FName("CombatTarget"), InstigatorPawn);
				}
			}

			// InstigatorPawn->ActorHasTag(ALinearPlayerCharacter::GetTag()) とどちらがいいだろう
			// 上記で設置した場合も Cast は必要だが、攻撃以外の要因でダメージを食らう場合は Tag で分けてもいい
			if (ALinearPlayerCharacter* LP_Character = Cast<ALinearPlayerCharacter>(InstigatorPawn))
			{
				// Delegate 登録
				LP_Character->OnCharacterDeathDelegate.AddUniqueDynamic(this, &AEnemyBase::ResetCharacterDie);
			}


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
	
	/*
	    UE_LOGFMT(LogTemp, Warning, "%f", Theta); エラーになる
		UE_LOGFMT(LogTemp, Warning, "{0}", Theta);

		if (GEngine)
			GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("%f"), Theta));

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
	if (CachedAIController == nullptr) return;

	UBlackboardComponent* BB = CachedAIController->GetBlackboardComponent();
	if (BB == nullptr) return;

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(FName("CombatTarget")));
	if (TargetActor == nullptr) return;

	FVector StartLocation = GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();

	// Z 軸は無視
	TargetLocation.Z = StartLocation.Z;

	// ターゲットの方向を向くための理想の角度を計算して、滑らかに振り向かせる
	FRotator TargetRotation = (TargetLocation - StartLocation).Rotation();
	FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), TargetRotation, DeltaTime, TrackingInterpSpeed);
	SetActorRotation(InterpRotation);

	//UE_LOGFMT(LogTemp, Warning, "TrackingRotation...");

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


void AEnemyBase::ResetCharacterDie()
{
	UE_LOGFMT(LogTemp, Warning, "AEnemyBase::ResetCharacterDie()");

	if (CachedAIController)
	{
		if (UBlackboardComponent* BB = CachedAIController->GetBlackboardComponent())
		{
			BB->SetValueAsBool(FName("IsTargetDied"), true);
		}
	}
}

