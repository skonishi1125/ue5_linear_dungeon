#include "Breakable/BreakableActor.h"
#include "Logging/StructuredLog.h"

#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Chaos/ChaosGameplayEventDispatcher.h"

//#include "Items/ItemBase.h"
#include "Components/LootDropComponent.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	// �j�Ђ� Pawn �Ɋ����Ȃ��悤�ɐݒ�
	// GC ���̂� Collision �͎Ւf���āABoxCollision ���� Block ����`���Ƃ�
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	// GC ���m�i��񂾔j�Ђ��ʂ� GC ��|���̂�h���j
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Destructible, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetNotifyBreaks(true);

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(GetRootComponent());
	BoxCollision->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);


	// Event Dispatcher �ݒ�
	GameplayEventDispatcher = CreateDefaultSubobject<UChaosGameplayEventDispatcher>(TEXT("GameplayEventDispatcher"));

	// �ŗL Component �����i�A�C�e���h���b�v�j
	LootDropComponent = CreateDefaultSubobject<ULootDropComponent>(TEXT("LootDropComponent"));
}

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();

	if (GeometryCollection)
	{
		GeometryCollection->OnChaosBreakEvent.AddDynamic(this, &ABreakableActor::OnChaosBreakEventBreakable);
	}
}

// Weapon, Rolling �Ȃǂŕ�����ꂽ�Ƃ��̋��ʏ���
void ABreakableActor::OnChaosBreakEventBreakable(const FChaosBreakEvent& BreakEvent)
{
	if (bIsBroken) return;
	bIsBroken = true;

	// �A�C�e���h���b�v ����
	if (LootDropComponent)
	{
		// ���g�̈ʒu��n���āA�h���b�v�������̂̓R���|�[�l���g�ɈϏ�
		LootDropComponent->ExecuteDrop(GetActorLocation(), GetActorRotation());
	}

	// BP ���ŁAActor ���Ƃ̌ŗL����
	// LifeSpan �̐ݒ�iC++ �ł������邪�f�U�C�i�[���M��z��Ƃ��āA�����瑤�ŏ����j
	// Pot �Ȃ�₪����鉹�A�V�����f���A�Ȃ� ���ꗎ���鉹�Ȃǂ̍Đ�
	OnBroken();
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Weapon ���� GetHit ���ĂԎ��̌ŗL�d�l�ݒ�p
void ABreakableActor::GetHit_Implementation(
	const FVector& ImpactPoint, const float PoiseDamage, bool bIsParry
)
{
	UE_LOGFMT(LogTemp, Warning, " ABreakableActor::GetHit_Implementation()");
	//if (DropItemClassToSpawn && GetWorld())
	//{
	//	const FVector SpawnLocation = GetActorLocation();
	//	const FRotator SpawnRotation = GetActorRotation();
	//	GetWorld()->SpawnActor<AItemBase>(DropItemClassToSpawn, SpawnLocation, SpawnRotation);
	//	UE_LOGFMT(LogTemp, Warning, "�A�C�e������");
	//}
}

