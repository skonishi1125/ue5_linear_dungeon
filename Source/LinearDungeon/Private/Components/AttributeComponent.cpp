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

void UAttributeComponent::TickComponent(
	float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);


	// ===== Poise 回復処理 =====
	// Poise が Max なら、Tick() を切って負荷を軽減する
	if (MaxPoise <= 0.f || CurrentPoise >= MaxPoise)
	{
		UpdatePoiseRecoveryTickEnabled();
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	// 例えば World 時間 100, 被弾時間 95 なら、 100 - 95 = 5 が返る
	const float TimeSinceDamage = World->GetTimeSeconds() - LastPoiseDamageTime;
	// Poise 回復までの指定時間より短ければ、何もしない（5秒に指定したなら、5秒間は回復処理をしない）
	if (TimeSinceDamage < PoiseRegenDelay)
	{
		return;
	}
	// 秒間回復量計算
	// Max = 100 で 5 秒で最大回復なら、100 / 5 = 20 ずつ回復していくことになる
	// KINDA_SMALL_NUMBER は、PoiseRegenDuration が誤って 0 に設定されていた時でも、
	// 強制的に極小値(0.001....)に差し替えてゼロ除算を防いでいる
	const float RegenRate = MaxPoise / FMath::Max(PoiseRegenDuration, KINDA_SMALL_NUMBER);

	// 計算した値を、Max まで上限として増やす
	// (DeltaTime を使って、フレームレートに依存せず増やしていく)
	CurrentPoise = FMath::Min(CurrentPoise + RegenRate * DeltaTime, MaxPoise);
	BroadcastPoisePercent(); // Poise が変わっていくことを Tick で通知
	UpdatePoiseRecoveryTickEnabled(); // CurrentPoise が Max になったら Tick() を切る
}

void UAttributeComponent::ReceiveHealthDamage(float Damage)
{
	// 0 - MaxHealth の間の数値で返す（ - になっても、Clamp して 0 を返すようにしている）
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
}

float UAttributeComponent::GetHealthPercent() const
{
	return MaxHealth > 0.f ? CurrentHealth / MaxHealth : 0.f;
}

bool UAttributeComponent::IsAlive() const
{
	return CurrentHealth > 0.f;
}

float UAttributeComponent::GetPoisePercent() const
{
	return MaxPoise > 0.f ? CurrentPoise / MaxPoise : 0.f;
}

void UAttributeComponent::ResetPoise()
{
	CurrentPoise = MaxPoise;
	LastPoiseDamageTime = 0.f; // 最後に被弾した時間をリセット
	UpdatePoiseRecoveryTickEnabled();
	BroadcastPoisePercent();
}

// Poise ダメージを受けたとき、Poise 回復用タイマーをセットして怯み判定の t/f を返す
bool UAttributeComponent::IsStaggeredWithPoise(float PoiseDamage)
{
	ApplyPoiseDamage(PoiseDamage);
	UE_LOGFMT(LogTemp, Warning, "UAttributeComponent::ReceivePoiseDamage() CurrentPoise: {0}", CurrentPoise);

	const bool bIsStaggered = CurrentPoise <= 0.f;
	if (!bIsStaggered)
	{
		// 怯まない被弾をしたとき、回復待ちタイマーをリセット（回復中の再被弾もここ）
		MarkPoiseDamaged();
		BroadcastPoisePercent();
	}

	return bIsStaggered;
}


void UAttributeComponent::ApplyPoiseDamage(float PoiseDamage)
{
	if (PoiseDamage <= 0.f || MaxPoise <= 0.f)
	{
		return;
	}
	CurrentPoise = FMath::Clamp(CurrentPoise - PoiseDamage, 0.f, MaxPoise);
}

// 最後に Poise を削った時間を変数に保管
void UAttributeComponent::MarkPoiseDamaged()
{
	if (UWorld* World = GetWorld())
	{
		LastPoiseDamageTime = World->GetTimeSeconds();
	}

	// Tick() のためのフラグを立てて、フレーム更新処理を有効化
	UpdatePoiseRecoveryTickEnabled();
}

// Tick() の ON / OFF を切り替える
void UAttributeComponent::UpdatePoiseRecoveryTickEnabled()
{
	// Poise が Max でないなら、回復処理が必要なのでフラグを建てる
	const bool bShouldTick = MaxPoise > 0.f && CurrentPoise < MaxPoise;
	SetComponentTickEnabled(bShouldTick);
}

void UAttributeComponent::BroadcastPoisePercent() const
{
	// const 関数の中からは、同じく const 指定された関数しか呼べない
	// ゲッタ関数にも const をつけておこう
	OnPoisePercentChanged.Broadcast(GetPoisePercent());
}
