#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LinearGameOverWidget.generated.h"

UCLASS()
class LINEARDUNGEON_API ULinearGameOverWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void PlayFadeInAnimation();
	void PlayTextFadeOutAnimation();

protected:
	// Animation ‚àA•Ï”‚Æ“¯‚¶‚æ‚¤‚É–¼‘O‚ğˆê’v‚³‚¹‚é
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> FadeInAnim;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> TextFadeOutAnim;
	
};
