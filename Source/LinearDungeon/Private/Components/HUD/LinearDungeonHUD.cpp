#include "Components/HUD/LinearDungeonHUD.h"
#include "Logging/StructuredLog.h"

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
			if (bStartHidden)
			{
				Overlay->SetVisibility(ESlateVisibility::Hidden);
			}
			Overlay->AddToViewport();
			UE_LOGFMT(LogTemp, Warning, "ALinearDungeonHUD::BeginPlay()");
		}
	}
	UE_LOGFMT(LogTemp, Warning, "ALinearDungeonHUD::BeginPlay() Finish");

}

void ALinearDungeonHUD::SetOverlayVisibility(bool bIsVisible)
{
	if (Overlay)
	{
		ESlateVisibility NewVisibility = bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
		Overlay->SetVisibility(NewVisibility);
		UE_LOGFMT(LogTemp, Warning, "ALinearDungeonHUD::SetOverlayVisibility {0}", bIsVisible);
	}
	else
	{
		UE_LOGFMT(LogTemp, Warning, "ALinearDungeonHUD::SetOverlayVisibility() No Overlay.");

	}
}
