#include "Characters/LinearPlayerAnimInstance.h"
#include "Characters/LinearPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h" // GroundSpeed 取得用

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
}
