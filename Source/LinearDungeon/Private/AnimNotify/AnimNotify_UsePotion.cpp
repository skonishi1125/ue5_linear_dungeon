#include "AnimNotify/AnimNotify_UsePotion.h"
#include "Characters/LinearPlayerCharacter.h"
#include "Enemies/EnemyBase.h"
#include "Components/InventoryComponent.h"

void UAnimNotify_UsePotion::Notify(
	USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, 
	const FAnimNotifyEventReference& EventReference
)
{
	Super::Notify(MeshComp, Animation, EventReference);

	// 敵味方どちらも使えるようにしたい
	if (MeshComp && MeshComp->GetOwner())
	{
		// EnemyBase, LinearPlayerCharacter どちらも共通の Entity クラスを作っていれば、if で分ける必要がなくなる
		// 今回は if で分ける
		//UAttributeComponent* Attributes = nullptr;
		//if (ALinearPlayerCharacter* LPCharacter = Cast<ALinearPlayerCharacter>(MeshComp->GetOwner()))
		//{
		//	Attributes = LPCharacter->GetAttributeComponent();
		//}
		//else if (AEnemyBase* EnemyBase = Cast<AEnemyBase>(MeshComp->GetOwner()))
		//{
		//	Attributes = EnemyBase->GetAttributeComponent();
		//}

		// ↑これでも良いが、GetComponentByClass で取ると、if で分ける必要がなくなる。


		AActor* OwnerActor = MeshComp->GetOwner();

		// Ownerが誰であろうと、UInventoryComponent を持っていれば取得する
		UInventoryComponent* Inventories = Cast<UInventoryComponent>(
			OwnerActor->GetComponentByClass(UInventoryComponent::StaticClass())
		);

		if (Inventories)
		{
			Inventories->UsePotion();
		}
	}

}
