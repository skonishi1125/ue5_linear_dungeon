#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LinearTitleMenuContainer.generated.h"

class UWidgetSwitcher;

UCLASS()
class LINEARDUNGEON_API ULinearTitleMenuContainer : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UWidgetSwitcher> MenuSwitcher;

	// Switch ‚·‚é WBP ‚ğ‘‚¢‚Ä‚¢‚­
	//UPROPERTY(meta = (BindWidget))
	//TObjectPtr<ULinearMainMenuUserWidget> WBP_LinearMainMenu;
	
};
