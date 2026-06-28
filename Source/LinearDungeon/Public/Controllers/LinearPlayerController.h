#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LinearPlayerController.generated.h"

// Controller が管理する Widget
class ULinearGameOverWidget;
class UMenuContainerWidget;
class ULinearRestartWidget;

// Enhanced Input
class UInputMappingContext;
class UInputAction;
class UEnhancedInputLocalPlayerSubsystem;

// 効果音
class USoundBase;

UCLASS()
class LINEARDUNGEON_API ALinearPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	// Enhanced Input から呼ぶ。現状 LPCharacter が持つため、public
	void ToggleMenu();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override; // キャラクターに Controller が付与したときの処理

	// ===== Widget =====
	UPROPERTY(EditAnywhere)
	TSubclassOf<UMenuContainerWidget> MenuContainerWidgetClass;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ULinearGameOverWidget> LinearGameOverWidgetClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ULinearRestartWidget> LinearRestartWidgetClass;

	// ===== Enhanced Input 関連 =====
	virtual void SetupInputComponent() override;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<class UInputMappingContext> DefaultMappingContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> MenuMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ToggleMenuAction;


private:
	UPROPERTY()
	TObjectPtr<UMenuContainerWidget> MenuContainerWidgetInstance;
	UPROPERTY()
	TObjectPtr<ULinearGameOverWidget> LinearGameOverWidgetInstance;
	UPROPERTY()
	TObjectPtr<ULinearRestartWidget> LinearRestartWidgetInstance;

	// ===== メニュー開閉関連 =====
	UFUNCTION()
	void CloseMenu();
	bool bIsMenuOpen = false;

	// ===== GameOver 関連 =====
	UFUNCTION()
	void OnPlayerDied();
	void ShowGameOverUI();
	void HideGameOverText();
	void RestartGame();

	FTimerHandle GameOverTimerHandle;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> GameOverSound;

	
};
