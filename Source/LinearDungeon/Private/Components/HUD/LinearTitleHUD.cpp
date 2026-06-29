#include "Components/HUD/LinearTitleHUD.h"
#include "Components/HUD/LinearTitleMenuContainer.h"

void ALinearTitleHUD::BeginPlay()
{
	Super::BeginPlay();

	// エディタ側でクラスが設定されていれば生成してViewportに追加
	if (LinearTitleMenuContainerClass)
	{
		LinearTitleMenuContainerInstance = CreateWidget<ULinearTitleMenuContainer>(GetWorld(), LinearTitleMenuContainerClass);
		if (LinearTitleMenuContainerInstance)
		{
			LinearTitleMenuContainerInstance->AddToViewport();
		}
	}
}
