#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractMarker.generated.h"

class UTextBlock;

// [E] 話す, [E] 調べる といった Widget を管理する
UCLASS()
class LINEARDUNGEON_API UInteractMarker : public UUserWidget
{
	GENERATED_BODY()
public:
	// 物なら「調べる」、人なら「話す」というような文字を適宜入れていく
	void SetInteractText(FText Text);
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InteractText;
	
};
