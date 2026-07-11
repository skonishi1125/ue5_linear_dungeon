#include "Managers/NpcPopManager.h"
#include "Logging/StructuredLog.h"

#include "Characters/LinearPlayerCharacter.h"

#include "Components/BoxComponent.h"
#include "Subsystems/LinearEventSubsystem.h" 
#include "Subsystems/LinearFlagSubsystem.h"

ANpcPopManager::ANpcPopManager()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	SetRootComponent(BoxCollision);
	BoxCollision->SetGenerateOverlapEvents(true);
	BoxCollision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void ANpcPopManager::BeginPlay()
{
	Super::BeginPlay();

	// 割り振られた EncounterID がクリア済みイベントかチェック
	if (UGameInstance* GI = GetGameInstance())
	{
		if (ULinearFlagSubsystem* FlagSubsystem = GI->GetSubsystem<ULinearFlagSubsystem>())
		{
			if (FlagSubsystem->CheckFlag(NpcPopID))
			{
				UE_LOGFMT(LogTemp, Warning, "ANpcPopManager::BeginPlay(): {0} is already set.", NpcPopID);
				DeleteTarget();
				return;
			}
		}
	}

	if (BoxCollision)
	{
		BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ANpcPopManager::OnBoxBeginOverlap);
	}
}

void ANpcPopManager::OnBoxBeginOverlap(
	UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
	bool bFromSweep, const FHitResult& SweepResult
)
{
	if (bIsDeleted) return;

	if (OtherActor && OtherActor->ActorHasTag(ALinearPlayerCharacter::GetTag()))
	{
		DeleteTarget();

		// クリア通知
		if (UGameInstance* GI = GetGameInstance())
		{
			if (ULinearFlagSubsystem* FlagSubsystem = GI->GetSubsystem<ULinearFlagSubsystem>())
			{
				FlagSubsystem->SetFlag(NpcPopID);
			}
		}
		//UE_LOG(LogTemp, Warning, TEXT("NPC Delete. Register FlagSubsystem"));
	}
}

void ANpcPopManager::DeleteTarget()
{
	for (AActor* Npc : TargetNpcs)
	{
		if (Npc)
		{
			Npc->Destroy();
		}
	}

	bIsDeleted = true;
}



