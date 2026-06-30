#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LinearTitleHUD.generated.h"

class ULinearTitleMenuContainer;

UCLASS()
class LINEARDUNGEON_API ALinearTitleHUD : public AHUD
{
	GENERATED_BODY()

public:
	ULinearTitleMenuContainer* GetMenuContainer() const { return LinearTitleMenuContainerInstance; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULinearTitleMenuContainer> LinearTitleMenuContainerClass;

	UPROPERTY()
	TObjectPtr<ULinearTitleMenuContainer> LinearTitleMenuContainerInstance;
	
};
