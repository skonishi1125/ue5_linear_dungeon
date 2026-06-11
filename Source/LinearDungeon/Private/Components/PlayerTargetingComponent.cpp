#include "Components/PlayerTargetingComponent.h"

UPlayerTargetingComponent::UPlayerTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UPlayerTargetingComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPlayerTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

