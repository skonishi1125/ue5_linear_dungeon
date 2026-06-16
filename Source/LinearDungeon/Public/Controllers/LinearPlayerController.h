#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LinearPlayerController.generated.h"

// Controller Ç™ä«óùÇ∑ÇÈ Widget
class ULinearMainMenuUserWidget;
class USaveLoadMenuWidget;

// Enhanced Input
class UInputMappingContext;
class UInputAction;

UCLASS()
class LINEARDUNGEON_API ALinearPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	// Enhanced Input Ç©ÇÁåƒÇ‘ÅBåªèÛ LPCharacter Ç™éùÇ¬ÇΩÇﬂÅApublic
	void ToggleMenu();

protected:
	virtual void BeginPlay() override;

	// ===== Widget =====
	UPROPERTY(EditAnywhere)
	TSubclassOf<ULinearMainMenuUserWidget> MainMenuWidgetClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<USaveLoadMenuWidget> SaveLoadMenuWidgetClass;

	// ===== Enhanced Input ä÷òA =====
	virtual void SetupInputComponent() override;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> MenuMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ToggleMenuAction;


private:
	UPROPERTY()
	TObjectPtr<ULinearMainMenuUserWidget> MainMenuWidgetInstance;
	UPROPERTY()
	TObjectPtr<USaveLoadMenuWidget> SaveLoadMenuWidgetInstance;

	bool bIsMenuOpen = false;


	
};
