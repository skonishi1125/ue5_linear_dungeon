#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"

#include "DoorBase.generated.h"

class UStaticMeshComponent;
class UBoxComponent;
class USoundBase;
class UArrowComponent;

UCLASS()
class LINEARDUNGEON_API ADoorBase : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	ADoorBase();
	virtual void Interact_Implementation(AActor* InstigatorActor) override;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;


	UFUNCTION()
	virtual void OnDoorBeginOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult
	);

	UFUNCTION()
	virtual void OnDoorEndOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex
	);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	TObjectPtr<UStaticMeshComponent> DoorMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	TObjectPtr<UStaticMeshComponent> RightDoorMesh; // 左右扉があるときの、右側の扉. null になり得る。
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	TObjectPtr<UBoxComponent> BoxComponent;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	TObjectPtr<UArrowComponent> DirectionReferenceArrow; // 内積を決めるための基準点
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Properties")
	TObjectPtr<USoundBase> DoorOpenSound;

private:

	// 状態管理フラグ
	bool bIsOpen;
	bool bIsMoving;

	// 角度管理
	float TargetYaw;
	float CurrentYaw;

	// 開く速度、角度
	UPROPERTY(EditDefaultsOnly, Category = "Door Settings")
	float OpenAngle = 90.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Door Settings")
	float RotationSpeed = 100.0f;

};
