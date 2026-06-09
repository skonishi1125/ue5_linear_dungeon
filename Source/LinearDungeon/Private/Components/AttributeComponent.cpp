#include "Components/AttributeComponent.h"
#include "Logging/StructuredLog.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;
	ResetPoise();
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttributeComponent::ReceiveHealthDamage(float Damage)
{
	// 0 - MaxHealth の間の数値で返す（ - になっても、Clamp して 0 を返すようにしている）
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
}

float UAttributeComponent::GetHealthPercent()
{
	return CurrentHealth / MaxHealth;
}

bool UAttributeComponent::IsAlive()
{
	return CurrentHealth > 0.f;
}

// 0以下になったら true を返して、怯みアニメ再生などの処理に移る
bool UAttributeComponent::IsStaggeredWithPoise(float PoiseDamage)
{
	CurrentPoise = FMath::Clamp(CurrentPoise - PoiseDamage, 0.f, MaxPoise);
	UE_LOGFMT(LogTemp, Warning, "UAttributeComponent::ReceivePoiseDamage() CurrentPoise: {0}", CurrentPoise);

	return CurrentPoise <= 0.f;
}

void UAttributeComponent::ResetPoise()
{
	CurrentPoise = MaxPoise;
}

