#include "Items/ItemBase.h"
#include "Logging/StructuredLog.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AItemBase::AItemBase()
{
	// 子クラス側で必要なものだけ、そちらで true にするようにする
	PrimaryActorTick.bCanEverTick = false;

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

}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	if (OverlapSphere)
	{
		OverlapSphere->OnComponentBeginOverlap.AddDynamic(this, &AItemBase::OnItemBeginOverlap);
	}
}

void AItemBase::OnActivated()
{
	UE_LOGFMT(LogTemp, Warning, "AItemBase::OnActivated()");
}

void AItemBase::OnItemBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOGFMT(LogTemp, Warning, "AItemBase::OnItemBeginOverlap()");
	//AItemBase::OnActivated(); クラスを明示すると、子クラスの処理が呼ばれない
	OnActivated();
}

//void AItemBase::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//}

