#include "Controllers/BootPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void ABootPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// ローディング UI 表示
	// AddToViewport を動作させるために Timer で数秒待機してから遷移を実行
	if (LoadingWidgetClass)
	{
		UUserWidget* LoadingWidget = CreateWidget<UUserWidget>(this, LoadingWidgetClass);
		if (LoadingWidget)
		{
			LoadingWidget->AddToViewport();
		}
	}

	GetWorld()->GetTimerManager().SetTimer(
		TransitionTimerHandle, this, &ABootPlayerController::ExecuteTransition, 5.f, false
	);
}

void ABootPlayerController::ExecuteTransition()
{
	if (!TitleLevel.IsNull())
	{
		UGameplayStatics::OpenLevelBySoftObjectPtr(this, TitleLevel);
	}
}