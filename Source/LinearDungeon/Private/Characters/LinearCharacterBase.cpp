#include "Characters/LinearCharacterBase.h"

ALinearCharacterBase::ALinearCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALinearCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void ALinearCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALinearCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

