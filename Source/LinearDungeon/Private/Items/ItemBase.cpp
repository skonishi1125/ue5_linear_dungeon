#include "Items/ItemBase.h"
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
}

//void AItemBase::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//}

