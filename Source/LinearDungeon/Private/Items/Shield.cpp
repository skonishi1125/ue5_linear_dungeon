#include "Items/Shield.h"
#include "Logging/StructuredLog.h"

#include "Characters/LinearPlayerCharacter.h"
#include "Components/SphereComponent.h" // OverlapSphere

// 音の再生関連
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

// Effect
#include "NiagaraComponent.h"

void AShield::Interact_Implementation(AActor* InstigatorActor)
{
	ALinearPlayerCharacter* PlayerCharacter = Cast<ALinearPlayerCharacter>(InstigatorActor);
	if (PlayerCharacter)
	{
		// プレイヤーに自身を装備させる
		PlayerCharacter->EquipShield(this);
	}
}

FText AShield::GetInteractPrompt_Implementation()
{
	// コンパイル保存したときにエラーが出た
	// Shield.cpp(20): error C2001: newline in constant
	// このファイル自体を Unicode (UTF-8 with signature) で保存すると通った。
	FString InteractText = FString::Printf(TEXT("[E] 装備"));
	return FText::FromString(InteractText);
}

void AShield::Equip(
	USceneComponent* InParent, FName InSocketName,
	AActor* NewOwner, APawn* NewInstigator, bool bPlaySound
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

void AShield::OnItemBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult
)
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

void AShield::OnItemEndOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex
)
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

void AShield::Drop(const FVector& DropLocation)
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
	if (NSEffect)
	{
		NSEffect->Activate();
	}
}
