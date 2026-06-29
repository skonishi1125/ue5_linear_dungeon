#include "Controllers/LinearTitlePlayerController.h"

void ALinearTitlePlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 入力モードをUI専用 (UI Only) に設定
	FInputModeUIOnly InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	// マウスカーソルを表示
	bShowMouseCursor = true;
}
