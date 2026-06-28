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
	// 無効ケースを除外
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
		return;
	}
	// ダメージ計算
	const float FinalDamage = BaseDamage * CurrentDamageMultiplier;
	const float FinalPoiseDamage = BasePoiseDamage * CurrentPoiseMultiplier;
	UE_LOGFMT(
		LogTemp, Warning,
		"AWeapon::OnBoxOverlap() finalDmg: {0} finalPoise: {1}",
		FinalDamage, FinalPoiseDamage
	);
	// Overlap相手に直接ダメージ
	AController* InstigatorController = nullptr;
	if (APawn* InstigatorPawn = GetInstigator())
	{
		InstigatorController = InstigatorPawn->GetController();
	}
	UGameplayStatics::ApplyDamage(
		OtherActor,
		FinalDamage,
		InstigatorController,
		this,
		UDamageType::StaticClass()
	);
	// ImpactPoint はSweepResult -> 補助Trace -> フォールバックの順で決定
	FVector ImpactPoint = SweepResult.ImpactPoint;
	if (ImpactPoint.IsNearlyZero())
	{
		const FVector Start = BoxTraceStart->GetComponentLocation();
		const FVector End = BoxTraceEnd->GetComponentLocation();
		TArray<AActor*> ActorsToIgnore;
		ActorsToIgnore.Add(this);
		if (AActor* OwnerActor = GetOwner())
		{
			ActorsToIgnore.Add(OwnerActor);
		}
		FHitResult BoxHit;
		const bool bTraceHit = UKismetSystemLibrary::BoxTraceSingle(
			this, Start,	End,
			FVector(8.f, 8.f, 8.f), // 位置補正用なので広めに取る
			BoxTraceStart->GetComponentRotation(),
			ETraceTypeQuery::TraceTypeQuery1,
			false,
			ActorsToIgnore,
			EDrawDebugTrace::None,
			BoxHit,
			true
		);

		// 補助 Trace は座標取得のみで、失敗してもダメージ判定には影響させない
		if (bTraceHit && BoxHit.GetActor() == OtherActor && !BoxHit.ImpactPoint.IsNearlyZero())
		{
			ImpactPoint = BoxHit.ImpactPoint;
		}
		else
		{
			ImpactPoint = (OtherComp != nullptr)
				? OtherComp->GetComponentLocation()
				: OtherActor->GetActorLocation();
		}
	}
	// 固有ヒット処理
	IHitInterface::Execute_GetHit(OtherActor, ImpactPoint, FinalPoiseDamage);
	// 同一攻撃中の多段防止
	BoxIgnoreActors.AddUnique(OtherActor);
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

