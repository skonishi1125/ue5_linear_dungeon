#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BootPlayerController.generated.h"

UCLASS()
class LINEARDUNGEON_API ABootPlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> LoadingWidgetClass;

	// ëJà⁄êÊÇÃTitleLevelÇéwíË
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Level")
	TSoftObjectPtr<UWorld> TitleLevel;

private:
	void ExecuteTransition();

	FTimerHandle TransitionTimerHandle;
};
