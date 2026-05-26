#include "Items/ItemBase.h"
#include "Logging/StructuredLog.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Characters/LinearPlayerCharacter.h"

#include "NiagaraComponent.h"

AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// USphereComponent 関連設定
	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	SetRootComponent(OverlapSphere);
	OverlapSphere->SetSphereRadius(50.f);
	OverlapSphere->SetGenerateOverlapEvents(true); // Overlap Event 有効化
	OverlapSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // 全てに対して感知させる

	// StaticMesh 関連設定
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	ItemMesh->SetupAttachment(GetRootComponent());
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Mesh 自体が Overlap に判定されないようにする

	// Effect
	NSEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NSEffect"));
	NSEffect->SetupAttachment(GetRootComponent());

}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	if (OverlapSphere)
	{
		OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnItemBeginOverlap);
		OverlapSphere->OnComponentEndOverlap.AddDynamic(this, &AItemBase::OnItemEndOverlap);
	}
}

void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;
	//UE_LOGFMT(LogTemp, Warning, "AItemBase::Tick() State: {ItemState}", static_cast<uint8>(ItemState));

	if (ItemState == EItemState::EIS_Dropped)
	{
		//UE_LOGFMT(LogTemp, Warning, "AItemBase::Tick() Drop");

		// 上下ホバリング
		HoveringObject();

		// 左回転処理
		RotateObject(DeltaTime);
	}
}


void AItemBase::HoveringObject()
{
	AddActorWorldOffset(
		FVector(0, 0, 
			Amplitude * FMath::Sin(RunningTime * TimeConstant)
		)
	);
}

void AItemBase::RotateObject(float DeltaTime)
{
	const float DeltaRotation = RotationSpeed * DeltaTime;
	AddActorLocalRotation(FRotator(.0f, DeltaRotation, .0f));
}

void AItemBase::OnItemBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOGFMT(LogTemp, Warning, "==PARENT== AItemBase::OnItemBeginOverlap()");
	//AItemBase::OnActivated(); クラスを明示すると、子クラスの処理が呼ばれない

	ALinearPlayerCharacter* LinearPlayerCharacter = Cast<ALinearPlayerCharacter>(OtherActor);
	if (LinearPlayerCharacter)
	{
		LinearPlayerCharacter->SetOverlappingItem(this);
	}

}

void AItemBase::OnItemEndOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex
)
{
	//UE_LOGFMT(LogTemp, Warning, "==PARENT== AItemBase::OnItemEndOverlap()");

	ALinearPlayerCharacter* LinearPlayerCharacter = Cast<ALinearPlayerCharacter>(OtherActor);
	if (LinearPlayerCharacter)
	{
		LinearPlayerCharacter->SetOverlappingItem(nullptr);
	}

}

