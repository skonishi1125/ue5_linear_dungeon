#include "Items/Shield.h"
#include "Logging/StructuredLog.h"

#include "Components/SphereComponent.h" // OverlapSphere

// 音の再生関連
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

// Effect
#include "NiagaraComponent.h"


void AShield::Equip(
	USceneComponent* InParent, FName InSocketName,
	AActor* NewOwner, APawn* NewInstigator
)
{
	UE_LOGFMT(LogTemp, Warning, "AShield::Equip()");
	SetOwner(NewOwner);
	SetInstigator(NewInstigator);

	// 盾 アタッチ
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	GetRootComponent()->AttachToComponent(InParent, TransformRules, InSocketName);
	ItemState = EItemState::EIS_Equipped;

	SetActorTickEnabled(false);

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