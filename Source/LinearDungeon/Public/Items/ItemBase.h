#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBase.generated.h"

class USphereComponent;
class UStaticMeshComponent;

UCLASS()
class LINEARDUNGEON_API AItemBase : public AActor
{
	GENERATED_BODY()
public:	
	AItemBase();
	//virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;
private:
	// 派生クラスから nullptr 等に設定されてしまうことを防ぐために private とする
	// BP で触れるようにしつつ, private に配置するために AllowPrivateAccess を設定
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> OverlapSphere; // USceneComponent を継承しているので、これを Root として扱っていく

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ItemMesh;

};
