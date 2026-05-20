#include "Items/Weapon.h"
#include "Logging/StructuredLog.h"

#include "Characters/LinearPlayerCharacter.h"

// 音の再生関連
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

// 判定関連
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"


AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	UE_LOGFMT(LogTemp, Warning, "Weapon BeginPlay");
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


void AWeapon::OnItemEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnItemEndOverlap(
		OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex
	);
	UE_LOGFMT(LogTemp, Warning, "AWeapon::OnItemEndOverlap");

}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName)
{
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

}

