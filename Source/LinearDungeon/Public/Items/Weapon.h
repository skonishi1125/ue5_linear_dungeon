#pragma once

#include "CoreMinimal.h"
#include "Items/ItemBase.h"
#include "Interfaces/InteractInterface.h"

#include "Weapon.generated.h"

class USceneComponent;
class USoundBase; // UMetaSound はこれを継承するので、このクラスとして格納できる
class UBoxComponent;

UCLASS()
class LINEARDUNGEON_API AWeapon : public AItemBase, public IInteractInterface
{
	GENERATED_BODY()
public:
	AWeapon();

	// InteractInterface 関連
	virtual void Interact_Implementation(AActor* InstigatorActor) override;
	virtual FText GetInteractPrompt_Implementation() override;

	void Equip(
		USceneComponent* InParent, FName InSocketName,
		AActor* NewOwner, APawn* NewInstigator
	);
	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; } // Character 側で操作する
	void SetMultipliers(float DamageMultiplier, float PoiseMultiplier);
	void ResetMultipliers();

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
	float BaseDamage = 20.f; // 基礎攻撃力
	float CurrentDamageMultiplier = 1.f;
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float BasePoiseDamage = 20.f; // 基礎ポイズ値
	float CurrentPoiseMultiplier = 1.f;


	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BoxTraceStart;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> BoxTraceEnd;

};
