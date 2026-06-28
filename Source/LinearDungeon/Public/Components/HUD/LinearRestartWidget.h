#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LinearRestartWidget.generated.h"

UCLASS()
class LINEARDUNGEON_API ULinearRestartWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void PlayFadeOutAnimation();

protected:
	// Animation ‚àA•Ï”‚Æ“¯‚¶‚æ‚¤‚É–¼‘O‚ğˆê’v‚³‚¹‚é
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> FadeOutAnim;
	
};
