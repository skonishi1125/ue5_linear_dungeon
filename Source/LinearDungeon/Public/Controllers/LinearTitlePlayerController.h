#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LinearTitlePlayerController.generated.h"

// Enhanced Input
class UInputMappingContext;
class UInputAction;
class UEnhancedInputLocalPlayerSubsystem;

UCLASS()
class LINEARDUNGEON_API ALinearTitlePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// ===== Enhanced Input 関連 =====
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> TitleMenuMappingContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> CloseTitleSubMenuAction;
	virtual void SetupInputComponent() override; // EnhancedInput Bindなど初期処理

private:
	// ===== メニュー開閉関連 =====
	UFUNCTION()
	void CloseTitleSubMenu();
	
};
