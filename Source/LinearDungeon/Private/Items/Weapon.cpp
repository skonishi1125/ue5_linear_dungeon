#include "Items/Weapon.h"
#include "Logging/StructuredLog.h"
#include "LinearDungeon/DebugMacros.h"

#include "Characters/LinearPlayerCharacter.h"

// 音の再生関連
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

// 判定関連
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h" // BoxTrace
#include "Interfaces/HitInterface.h"

// Effect
#include "NiagaraComponent.h"

AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());

	// 武器判定設定
	// Pawn にだけは Overlap 検知に反応しないように設定
	//WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 通常時は NoCollision
	//WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	//WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	// ↑を変えた
	// 通常時は NoCollsion (同じ), Overlap イベントのチャンネルをセット(同じ)
	// Pawn (Capsule) に対しても Overlap イベントを拾えるようにして、相手のメッシュ依存でなくする
	// 設定の適用
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 通常時は NoCollision
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	WeaponBox->SetGenerateOverlapEvents(true);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());
	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());

	LeftHandGrip = CreateDefaultSubobject<USceneComponent>(TEXT("LeftHandGrip"));
	LeftHandGrip->SetupAttachment(GetRootComponent());

}

void AWeapon::Interact_Implementation(AActor* InstigatorActor)
{
	ALinearPlayerCharacter* PlayerCharacter = Cast<ALinearPlayerCharacter>(InstigatorActor);
	if (PlayerCharacter)
	{
		// プレイヤーに自身を装備させる
		PlayerCharacter->EquipWeapon(this);
	}
}

FText AWeapon::GetInteractPrompt_Implementation()
{
	FString InteractText = FString::Printf(TEXT("[E] 装備"));
	return FText::FromString(InteractText);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	UE_LOGFMT(LogTemp, Warning, "Weapon BeginPlay");
	if (WeaponBox)
	{
		WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
	}

}

void AWeapon::OnItemBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnItemBeginOverlap(
		OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult
	);

	ALinearPlayerCharacter* LinearPlayerCharacter = Cast<ALinearPlayerCharacter>(OtherActor);
	if (LinearPlayerCharacter)
	{
		LinearPlayerCharacter->SetOverlappingInteractableActor(this);
	}
}


void AWeapon::OnItemEndOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnItemEndOverlap(
		OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex
	);

	ALinearPlayerCharacter* LinearPlayerCharacter = Cast<ALinearPlayerCharacter>(OtherActor);
	if (LinearPlayerCharacter)
	{
		LinearPlayerCharacter->SetOverlappingInteractableActor(nullptr);
	}

}

void AWeapon::OnBoxOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
	// 武器を所持する Player に当たった場合は無効
	if (OtherActor == nullptr || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}
	// ヒット対象を明確化（HitInterface実装Actorのみ）
	if (!OtherActor->GetClass()->ImplementsInterface(UHitInterface::StaticClass()))
	{
		return;
	}
	// 多段ヒット防止
	if (BoxIgnoreActors.Contains(OtherActor))
	{
		UE_LOGFMT(LogTemp, Warning, "Ignore!");
		return;
	}

	// ImpactPoint 計算
	const FVector ImpactPoint = ResolveImpactPoint(
		BoxTraceStart, BoxTraceEnd,
		OverlappedComponent, OtherComp, bFromSweep, SweepResult
	);

	// ダメージ計算
	ApplyMeleeHit(OtherActor, ImpactPoint);
	
}

FVector AWeapon::ResolveImpactPoint(
	USceneComponent* TraceStart, USceneComponent* TraceEnd,
	UPrimitiveComponent* AttackBox, UPrimitiveComponent* OtherComp,
	bool bFromSweep, const FHitResult& SweepResult
)
{
	// Trace で正確な接触点の取得を試す
	if (TraceStart && TraceEnd)
	{
		TArray<AActor*> Ignore; Ignore.Add(this);
		FHitResult BoxHit;
		const bool bHit = UKismetSystemLibrary::BoxTraceSingle(
			this, TraceStart->GetComponentLocation(), TraceEnd->GetComponentLocation(),
			FVector(5.f), TraceStart->GetComponentRotation(),
			ETraceTypeQuery::TraceTypeQuery1, false, Ignore,
			EDrawDebugTrace::None, BoxHit, true
		);
		if (bHit)
		{
			return BoxHit.ImpactPoint;
		}
	}

	// Overlap が Sweep から取得できたならそちらを使う
	if (bFromSweep && !SweepResult.ImpactPoint.IsZero())
	{
		return SweepResult.ImpactPoint;
	}

	// Trace 等で取得できなければ、攻撃判定の Box に一番近い表面上の点を取得
	if (OtherComp)
	{
		FVector Closest;
		if (OtherComp->GetClosestPointOnCollision(AttackBox->GetComponentLocation(), Closest) > 0.f)
		{
			return Closest;
		}
	}

	// それでも取得できなければ、Box 自体の点を ImpacePoint とする
	return AttackBox->GetComponentLocation();

}

void AWeapon::ApplyMeleeHit(AActor* HitActor, const FVector& ImpactPoint)
{
	if (!HitActor || BoxIgnoreActors.Contains(HitActor))
	{
		return;
	}

	const float FinalDamage = BaseDamage * CurrentDamageMultiplier;
	const float FinalPoiseDamage = BasePoiseDamage * CurrentPoiseMultiplier;

	UGameplayStatics::ApplyDamage(HitActor, FinalDamage, GetInstigator()->GetController(), this, UDamageType::StaticClass());

	if (IHitInterface* HitInterface = Cast<IHitInterface>(HitActor))
	{
		HitInterface->Execute_GetHit(HitActor, ImpactPoint, FinalPoiseDamage, false);
	}

	// 同一攻撃中の多段ヒット防止
	BoxIgnoreActors.AddUnique(HitActor);

	// 破壊フィールド生成
	CreateFields(ImpactPoint);

}

void AWeapon::Equip(
	USceneComponent* InParent, FName InSocketName,
	AActor* NewOwner, APawn* NewInstigator, bool bPlaySound
)
{
	// Owner, Instigator 設定
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);

	// 武器のアタッチ先指定
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	GetRootComponent()->AttachToComponent(InParent, TransformRules, InSocketName);
	ItemState = EItemState::EIS_Equipped;

	// 装備後の設定
	// Tick 無効化
	// PrimaryActorTick.bCanEverTick は UE に Tick するオブジェクトかどうかを伝えるものなので、こちらで操作はできない
	SetActorTickEnabled(false);

	// Collision を無効化して、Character と擦れる度に発生する Overlap イベントを防ぐ
	if (OverlapSphere)
	{
		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 音再生
	if (EquipSound && bPlaySound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}

	// エフェクトを切る
	if (NSEffect)
	{
		NSEffect->Deactivate();
	}

}

void AWeapon::SetMultipliers(float DamageMultiplier, float PoiseMultiplier)
{
	CurrentDamageMultiplier = DamageMultiplier;
	CurrentPoiseMultiplier = PoiseMultiplier;
}

void AWeapon::ResetMultipliers()
{
	CurrentDamageMultiplier = 1.f;
	CurrentPoiseMultiplier = 1.f;
}

void AWeapon::Drop(const FVector& DropLocation)
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetOwner(nullptr);
	SetInstigator(nullptr);
	SetActorLocation(DropLocation);
	ItemState = EItemState::EIS_Dropped;
	SetActorTickEnabled(true);
	if (OverlapSphere)
	{
		OverlapSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	if (WeaponBox)
	{
		WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	if (NSEffect)
	{
		NSEffect->Activate();
	}
}

