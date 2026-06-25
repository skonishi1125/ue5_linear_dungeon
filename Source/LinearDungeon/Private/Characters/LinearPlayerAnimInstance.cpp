#include "Characters/LinearPlayerAnimInstance.h"
#include "Characters/LinearPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/Weapon.h"

#include "Kismet/KismetMathLibrary.h" // GroundSpeed 取得用
#include "Characters/CharacterTypes.h"

void ULinearPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	LinearPlayerCharacter = Cast<ALinearPlayerCharacter>(TryGetPawnOwner());
	if (LinearPlayerCharacter)
	{
		LinearPlayerCharacterMovement = LinearPlayerCharacter->GetCharacterMovement();
	}
}

// Animation 中、監視したい項目を書く
void ULinearPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (LinearPlayerCharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(LinearPlayerCharacterMovement->Velocity);
		IsFalling = LinearPlayerCharacterMovement->IsFalling();

		// キャラの State を常に監視する
		// 武器の所持 / 非所持 などでアニメをを使い分けるため
		CharacterState = LinearPlayerCharacter->GetCharacterState();
	}

	if (LinearPlayerCharacter && LinearPlayerCharacter->GetCharacterActionState() == EActionState::EAS_Defensing)
	{
		bIsDefensing = true;
	}
	else
	{
		bIsDefensing = false;
	}

	// 長物を持っているときの左手補正
	LeftHandIKAlpha = 0.f;
	if (LinearPlayerCharacter && 
		LinearPlayerCharacter->GetCharacterState() == ECharacterState::ECS_EquippedTwoHandedWeapon && 
		! IsFalling && // ジャンプ中は長物でも補正しない
		LinearPlayerCharacter->GetEquippedWeapon()
	)
	{
		AWeapon* EquippedWeapon = LinearPlayerCharacter->GetEquippedWeapon();
		if (EquippedWeapon->GetLeftHandGrip())
		{
			// 武器の LeftHandGrip の World Space(レベル基準) 座標 を取得
			const FVector GripWorldLocation = EquippedWeapon->GetLeftHandGrip()->GetComponentLocation();

			// GetMesh()->GetComponentTransform() でキャラクターメッシュの World Transform（位置・回転・スケール）を取得
			// メッシュの座標を基準に、引数として渡した座標を InverseTransformPosition() で Component Space 座標に変換
			// (メッシュ座標を原点として、そちらから見て LeftHandIKLocation はどうかという値に変換）
			// この座標を ABP の IK Two Bone 側の Effector Location (指定したBoneをどこに持っていくかの設定値)に設定する
			LeftHandIKLocation = LinearPlayerCharacter->GetMesh()->GetComponentTransform().InverseTransformPosition(GripWorldLocation);

			// Alpha: 適用値 1 が最大, 0 が無し。両手武器の時だけ適用するという意味になる
			LeftHandIKAlpha = 1.f;
		}
	}


}
