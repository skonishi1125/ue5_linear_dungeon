#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuitGameWidget.generated.h"

class UButton;

// MenuContainer Ç…å¸ÇØÇƒÅAí ímÇîÚÇŒÇ∑
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReturnMainMenuRequestedDelegate);


UCLASS()
class LINEARDUNGEON_API UQuitGameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnReturnMainMenuRequestedDelegate OnReturnMainMenuRequestedDelegate;

protected:
	virtual bool Initialize() override;
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NoButton;
	UFUNCTION()
	void OnNoButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> YesButton;
	UFUNCTION()
	void OnYesButtonClicked();

};
