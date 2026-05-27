#pragma once

#include "CoreMinimal.h"
#include "Items/ItemBase.h"
#include "Weapon.generated.h"

class USceneComponent;
class USoundBase; // UMetaSound はこれを継承するので、このクラスとして格納できる
class UBoxComponent;

UCLASS()
class LINEARDUNGEON_API AWeapon : public AItemBase
{
	GENERATED_BODY()
public:
	AWeapon();
	void Equip(
		USceneComponent* InParent, FName InSocketName,
		AActor* NewOwner, APawn* NewInstigator
	);
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; } // Character 側で操作する
	// 武器判定で考慮しない Actor の配列
	TArray<AActor*> BoxIgnoreActors; // 攻撃モーション中の一時的なリストなので、生ポインタで良い

protected:
	virtual void BeginPlay() override;

	// アイテム取得感知
	// 親側で UFUNCTION() を付与していたら、書かなくてよい
	// アクセス権は親が protected なので、そちらに合わせるのが基本的な考え方
	virtual void OnItemBeginOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult
	) override;

	virtual void OnItemEndOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex
	) override;

	// 武器判定感知
	UFUNCTION()
	void OnBoxOverlap(
		UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult
	);

	UFUNCTION(BlueprintImplementableEvent)
	void CreateFields(const FVector& FieldLocation);

private:
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	TObjectPtr<USoundBase> EquipSound;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	TObjectPtr<UBoxComponent> WeaponBox;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 20.f; // 武器それぞれの攻撃力

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BoxTraceEnd;

};
