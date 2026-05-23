#include "Enemies/EnemyBase.h"
#include "Logging/StructuredLog.h"
#include "LinearDungeon/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h" // DrawDebugAllow

// 音関連
#include "Sound/SoundBase.h"
#include "Kismet/GameplayStatics.h"

#include "Components/CapsuleComponent.h"

AEnemyBase::AEnemyBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// WeaponBox の判定と感知するように, また カメラに干渉しないように設定
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
}

void AEnemyBase::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemyBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemyBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemyBase::GetHit(const FVector& ImpactPoint)
{
	LOG_WARN("AEnemyBase::GetHit()");
	//DRAW_SPHERE_COLOR(ImpactPoint, FColor::Orange);

	DirectionalHitReact(ImpactPoint);

	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, ImpactPoint);
	}

	if (HitParticle && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), HitParticle, ImpactPoint
		);
	}

}

void AEnemyBase::DirectionalHitReact(const FVector& ImpactPoint)
{
	// 当たった角度に応じて再生する Montage を決定する
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();

	// 角度を内積を用いて導く
	// Forward * ToHit = |Forward| |ToHit| * cosθ
	// 正規化しているので、実質 = cosθ である
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheta); // cosθ から、θ(角度)だけを逆三角関数を用いて取り出す
	Theta = FMath::RadiansToDegrees(Theta);

	// 導いた角度が + か - かを、外積で出す
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	//UE_LOGFMT(LogTemp, Warning, "%f", Theta); エラーになる
	/*
		UE_LOGFMT(LogTemp, Warning, "{0}", Theta);

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("%f"), Theta));
		}


		UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);
		UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + ToHit * 60.f, 5.f, FColor::Green, 5.f);
		UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(), GetActorLocation() + CrossProduct * 60.f, 5.f, FColor::Blue, 5.f);
	*/

	// 角度に応じて再生する Montage を決める
	// ↓ を正面としたとき、←が + →が - の角度

	FName Section("FromBack");
	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}

	PlayHitReactionMontage(Section);
}

void AEnemyBase::PlayHitReactionMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactionMontage)
	{
		AnimInstance->Montage_Play(HitReactionMontage, 1.0f, EMontagePlayReturnType::MontageLength, .0f, true);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactionMontage);
	}

}

