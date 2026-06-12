#include "Components/PlayerTargetingComponent.h"
#include "Logging/StructuredLog.h"

#include "Enemies/EnemyBase.h"
#include "Components/AttributeComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/SpringArmComponent.h"

UPlayerTargetingComponent::UPlayerTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPlayerTargetingComponent::BeginPlay()
{
	Super::BeginPlay();
	SetComponentTickEnabled(false);
}



void UPlayerTargetingComponent::SetSpringArm(USpringArmComponent* SpringArm)
{
	CachedPlayerSpringArm = SpringArm;
	if (CachedPlayerSpringArm)
	{
		DefaultArmLength = CachedPlayerSpringArm->TargetArmLength;
		DefaultArmRotation = CachedPlayerSpringArm->GetRelativeRotation();
	}
}

void UPlayerTargetingComponent::ApplySpringArmProfile()
{
	if (CachedPlayerSpringArm == nullptr) return;

	CachedPlayerSpringArm->TargetArmLength = LockOnArmLength;
	// Player 側で、SpringArm->bUsePawnControlRotation = true; としているので、設定してもすぐ戻る
	//CachedPlayerSpringArm->SetRelativeRotation(LockOnArmRotation);

}

void UPlayerTargetingComponent::RestoreSpringArmProfile()
{
	if (CachedPlayerSpringArm == nullptr) return;

	CachedPlayerSpringArm->TargetArmLength = DefaultArmLength;
	//CachedPlayerSpringArm->SetRelativeRotation(DefaultArmRotation);
}


void UPlayerTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bIsLocked)
	{
		IsTargetValid();
	}
}

// Tick 中、Target へのカメラを有効とすべきかどうかをチェックする関数
void UPlayerTargetingComponent::IsTargetValid()
{
	// Component, CurrentTarget の取得が失敗したとき、解除
	const AActor* Owner = GetOwner();
	AActor* Target = CurrentTarget.Get();
	if (!Target || !Owner)
	{
		ClearLock();
		return;
	}

	// CurrentTarget が死亡したら、解除
	if (const AEnemyBase* Enemy = Cast<AEnemyBase>(Target))
	{
		if (const UAttributeComponent* Attr = Enemy->FindComponentByClass<UAttributeComponent>())
		{
			if (!Attr->IsAlive())
			{
				ClearLock();
				return;
			}
		}
	}

	// 距離が一定以上離れても、解除 ※DistSq = 二乗の結果値
	const float DistSq = FVector::DistSquared(
		Owner->GetActorLocation(), CurrentTarget->GetActorLocation()
	);

	//UE_LOGFMT(LogTemp, Warning, "{0}", DistSq);

	// 二乗で計算しているので、MaxLockDitance も二乗で比較する
	if (DistSq > MaxLockDistance * MaxLockDistance)
	{
		ClearLock();
	}

}


// ロックオン押下 / 解除時の設定
void UPlayerTargetingComponent::OnLockOnPressed()
{

	// 既にロック済なら解除
	if (bIsLocked)
	{
		ClearLock();
		return;
	}

	// 索敵範囲に対象がいなければ何もしない
	AActor* Best = FindBestTarget();
	if (!Best)
	{
		UE_LOGFMT(LogTemp, Warning, "No Target");
		return;
	}

	// ロックオン処理
	CurrentTarget = Best;// 現在注目中の敵のポインタ格納
	bIsLocked = true;// ロック中フラグ有効化
	SetComponentTickEnabled(true);// Tick 有効化（敵を追従するような処理）
	ApplySpringArmProfile(); // SpringArm の設定を、ロック中専用のものに更新

	//DrawDebugSphere(GetWorld(), CurrentTarget->GetActorLocation(), MaxLockDistance, 12, FColor::Red, true);
	//UE_LOGFMT(LogTemp, Warning, "Target: {0}", CurrentTarget->GetActorLabel());
	//UE_LOGFMT(LogTemp, Warning, "UPlayerTargetingComponent::OnLockOnPressed() bIslocked = {0}", bIsLocked ? TEXT("true") : TEXT("false"));
}

void UPlayerTargetingComponent::ClearLock()
{
	bIsLocked = false;
	CurrentTarget = nullptr;
	SetComponentTickEnabled(false); // Tick も切る
	RestoreSpringArmProfile(); // SpringArm の設定も、元々のものに
	UE_LOGFMT(LogTemp, Warning, "ClearLock(): Target Remove");
}


// 自分の中心から球上にサーチして、条件に合う Actor を返す関数
AActor* UPlayerTargetingComponent::FindBestTarget() const
{
	const AActor* Owner = GetOwner();
	const UWorld* World = GetWorld();
	if (!Owner || !World)
	{
		return nullptr;
	}

	// 作り出す球の中にいた Actor 全てを格納するための配列
	TArray<FOverlapResult> Overlaps;

	// SCENE_QUERY_STAT(LockOnOverlap): デバッグ、プロファイル用のラベル設定
	// false: 物理クエリでないことの宣言（軽くする）
	// Owner: 自身を設定して、自分自身は ヒット対象から省く
	FCollisionQueryParams Params(SCENE_QUERY_STAT(LockOnOverlap), false, Owner);

	// SphereComponent の Overlap などが有名だが、
	// このように、関数を呼んだ時だけ仮想の球を作り出して、Overlap したものを取得したりすることもできる
	// (Weapon のように触れているものを常時チェックする場合などは、↑の手法を使う)
	const bool bHit = World->OverlapMultiByChannel(
		Overlaps,
		Owner->GetActorLocation(),
		FQuat::Identity,
		ECC_Pawn,// 敵も Pawn なのでこれで拾える
		FCollisionShape::MakeSphere(MaxLockDistance),
		Params
	);

	if (!bHit)
	{
		return nullptr;
	}

	// 対象を格納する Actor ポインタ
	AActor* BestTarget = nullptr;

	// float で表せる最大値が UE に用意されているので、最大値を入れておく
	// （後で、最も近い Actor を仕分けるために使う）
	float BestDistSq = MAX_FLT;

	for (const FOverlapResult& Result : Overlaps)
	{
		AActor* Candidate = Result.GetActor();
		if (!Candidate || Candidate == Owner)
		{
			continue;
		}
		// EnemyBase だけを対象とする
		const AEnemyBase* Enemy = Cast<AEnemyBase>(Candidate);
		if (!Enemy)
		{
			continue;
		}

		// 死んでたら除外
		if (const UAttributeComponent* Attr = Enemy->FindComponentByClass<UAttributeComponent>())
		{
			if (!Attr->IsAlive())
			{
				continue;
			}
		}

		// A-B 間の距離の二乗を返す
		const float DistSq = FVector::DistSquared(
			Owner->GetActorLocation(), Candidate->GetActorLocation()
		);

		// 今まで計測してきた最長の距離の Actor と比較して、近ければ新しい候補者として格納する
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestTarget = Candidate;
		}
	}

	return BestTarget;
}


// ターゲット中、常に対象のほうを向く処理
// LinearPlayerCharacter 側の Tick で使う
void UPlayerTargetingComponent::UpdateLockOnCamera(USpringArmComponent* SpringArm, float DeltaTime)
{
	if (!bIsLocked || !SpringArm)
	{
		return;
	}

	if (CachedPlayerSpringArm == nullptr)
	{
		CachedPlayerSpringArm = SpringArm;
	}

	AActor* Target = CurrentTarget.Get();
	AActor* Owner = GetOwner();
	if (!Target || !Owner)
	{
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(Owner);
	if (!OwnerPawn)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC)
	{
		return;
	}

	// プレイヤーと敵の 3D World 座標取得
	const FVector OwnerLocation = Owner->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	// Z軸を均一化して高低差を無くし、計算しやすくしておく
	TargetLocation.Z = OwnerLocation.Z;

	// プレイヤー(A) から 敵(B) への Rotation を取る B - A
	FRotator DesiredRotation = (TargetLocation - OwnerLocation).Rotation();
	// 計算のために統一した Z 軸を、希望の設定値に代入
	// Pitch: 上下 / Yaw: 左右 / Roll: 傾き
	DesiredRotation.Pitch = LockOnArmRotation.Pitch;

	// 現時点の Rotation から、NewRotation に向かって、Interpを使って補間
	// (ワープするような移動ではなく、ゆっくり遷移するように移動していく形をとる)
	const FRotator CurrentRotation = PC->GetControlRotation();
	const FRotator NewRotation = FMath::RInterpTo(
		CurrentRotation,
		DesiredRotation,
		DeltaTime,
		CameraRotationInterpSpeed
	
	);

	// 導出した NewRotation を Controller への回転適用
	// Tick で動かすので、Interp で徐々に動くような挙動になる
	PC->SetControlRotation(NewRotation);
}





