#include "MeshActors/DoorBase.h"
#include "Logging/StructuredLog.h"
#include "LinearDungeon/DebugMacros.h"

#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"
#include "Characters/LinearPlayerCharacter.h"

// 音の再生関連
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

ADoorBase::ADoorBase()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// 回転軸を調整できるよう、空の親を用意する
	USceneComponent* SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(SceneRoot);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMeshComponent"));
	DoorMesh->SetupAttachment(GetRootComponent());
	DoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	RightDoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightDoorMeshComponent"));
	RightDoorMesh->SetupAttachment(GetRootComponent());
	RightDoorMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	DirectionReferenceArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("DirectionReferenceArrow"));
	DirectionReferenceArrow->SetupAttachment(GetRootComponent());
	DirectionReferenceArrow->ArrowColor = FColor::Red;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(GetRootComponent());
	BoxComponent->SetGenerateOverlapEvents(true); // Overlap Event 有効化
	BoxComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // 全てに対して感知させる

}

void ADoorBase::Interact_Implementation(AActor* InstigatorActor)
{
	UE_LOGFMT(LogTemp, Warning, "ADoorBase::Interact_Implementation");

	if (bIsOpen) return;

	// 目標の角度を設定
	bIsOpen = true;
	bIsMoving = true;

	float DirectionMultiplier = 1.0f;

	if (InstigatorActor)
	{
		// アローコンポーネントの正面ベクトル（ドアの正面とする場所に、別途 BP で配置する）を取得
		const FVector ReferenceForward = DirectionReferenceArrow->GetForwardVector();

		// Player から Door へのベクトル
		FVector DirFromInstigator = GetActorLocation() - InstigatorActor->GetActorLocation();
		DirFromInstigator.Z = 0.0f;
		DirFromInstigator.Normalize();

		const float DotResult = FVector::DotProduct(ReferenceForward, DirFromInstigator);

		// アローが手前 プレイヤーが手前 なら プラス、 アローが手前 プレイヤーが後ろ なら マイナス
		// + は -1.0f を掛けて反時計回りに、- は 1.0f を掛けて時計回りに開かせる
		DirectionMultiplier = (DotResult > 0.0f) ? -1.0f : 1.0f;
	}

	TargetYaw = OpenAngle * DirectionMultiplier;
	SetActorTickEnabled(true); // 回転処理のため Tick 有効化

	if (DoorOpenSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DoorOpenSound, GetActorLocation());
	}
}

void ADoorBase::BeginPlay()
{
	Super::BeginPlay();
	if (BoxComponent)
	{
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ADoorBase::OnDoorBeginOverlap);
		BoxComponent->OnComponentEndOverlap.AddDynamic(this, &ADoorBase::OnDoorEndOverlap);
	}

}

void ADoorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsMoving)
	{
		// DeltaTime で現在の角度から目標角度へ動かす
		// 単純な Clamp などだと、フレームによって速度が変わるのでこういったケースは Interp が良い
		CurrentYaw = FMath::FInterpConstantTo(CurrentYaw, TargetYaw, DeltaTime, RotationSpeed);

		FRotator NewRotation = FRotator(0.0f, CurrentYaw, 0.0f);
		FRotator MinusNewRotation = FRotator(0.0f, -CurrentYaw, 0.0f);
		DoorMesh->SetRelativeRotation(NewRotation); // Z 軸 Yaw を中心に回転

		if (RightDoorMesh)
		{
			RightDoorMesh->SetRelativeRotation(MinusNewRotation);
		}

		// 目標角度に到達したかの判定 (誤差を考慮して IsNearlyEqual を使用)
		if (FMath::IsNearlyEqual(CurrentYaw, TargetYaw, 0.1f))
		{
			// ズレを修正して補正し、Tick を止める
			DoorMesh->SetRelativeRotation(FRotator(0.0f, TargetYaw, 0.0f));
			CurrentYaw = TargetYaw;

			bIsMoving = false;
			SetActorTickEnabled(false);
		}
	}

}

void ADoorBase::OnDoorBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult
)
{
	if (bIsOpen) return;

	ALinearPlayerCharacter* LinearPlayerCharacter = Cast<ALinearPlayerCharacter>(OtherActor);
	if (LinearPlayerCharacter)
	{
		LinearPlayerCharacter->SetOverlappingInteractableActor(this);
	}
}

void ADoorBase::OnDoorEndOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex
)
{
	if (bIsOpen) return;

	ALinearPlayerCharacter* LinearPlayerCharacter = Cast<ALinearPlayerCharacter>(OtherActor);
	if (LinearPlayerCharacter)
	{
		LinearPlayerCharacter->SetOverlappingInteractableActor(nullptr);
	}
}

