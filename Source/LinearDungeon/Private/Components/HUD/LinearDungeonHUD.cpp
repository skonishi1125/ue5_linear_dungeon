#include "Components/HUD/LinearDungeonHUD.h"
#include "Components/HUD/LinearDungeonOverlay.h"

void ALinearDungeonHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && LinearDungeonOverlayClass)
		{
			Overlay = CreateWidget<ULinearDungeonOverlay>(Controller, LinearDungeonOverlayClass);
			Overlay->AddToViewport();
		}
	}


}
