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
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision); // 通常時は NoCollision
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());

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
	UE_LOGFMT(LogTemp, Warning, "AWeapon::OnItemBeginOverlap");
}


void AWeapon::OnItemEndOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnItemEndOverlap(
		OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex
	);
	UE_LOGFMT(LogTemp, Warning, "AWeapon::OnItemEndOverlap");

}

void AWeapon::OnBoxOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult)
{
	// BoxTrace 始点と終点
	const FVector Start = BoxTraceStart->GetComponentLocation();
	const FVector End = BoxTraceEnd->GetComponentLocation();

	// 無視する対象
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	for (AActor* Actor : BoxIgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	FHitResult BoxHit;

	UKismetSystemLibrary::BoxTraceSingle(
		this, Start, End, FVector(5.f, 5.f, 5.f),
		BoxTraceStart->GetComponentRotation(), ETraceTypeQuery::TraceTypeQuery1, false,
		ActorsToIgnore, 
		EDrawDebugTrace::None, // Debug 表示
		BoxHit, // Hit した情報を指定の変数に格納 (& で参照渡しの形になっている)
		true
	);

	if (BoxHit.GetActor())
	{
		// 1. ダメージ処理
		UGameplayStatics::ApplyDamage(
			BoxHit.GetActor(),
			Damage,
			GetInstigator()->GetController(),
			this,
			UDamageType::StaticClass()
		);

		// 2.Interface に応じた固有処理
		IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
		if (HitInterface)
		{
			//HitInterface->GetHit(BoxHit.ImpactPoint);
			HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint);
		}
		// 武器を振った時、同じ敵に複数回当たらないようにする
		// 武器判定を Enabled / Disabled とするとき、リセットするようにする (Character側)
		BoxIgnoreActors.AddUnique(BoxHit.GetActor());

		// Geometry Collections 等を破壊するための力 Field 作成
		CreateFields(BoxHit.ImpactPoint);


	}
	
}

void AWeapon::Equip(
	USceneComponent* InParent, FName InSocketName,
	AActor* NewOwner, APawn* NewInstigator
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
	if (EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, EquipSound, GetActorLocation());
	}

	// エフェクトを切る
	if (NSEffect)
	{
		NSEffect->Deactivate();
	}

}

