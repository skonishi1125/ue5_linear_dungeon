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

void ULinearPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (LinearPlayerCharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(LinearPlayerCharacterMovement->Velocity);
		IsFalling = LinearPlayerCharacterMovement->IsFalling();
	}
}
