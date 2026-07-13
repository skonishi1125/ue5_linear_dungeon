#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LinearGameMode.generated.h"

// EBGMT_Title...とする書き方をプロジェクト内で採用しているが、
// 下記の書き方でもいいようなので、今回はこちらで書いてみる
UENUM(BlueprintType)
enum class EBGMType : uint8
{
    Opening UMETA(DisplayName = "Opening"),
    MainDungeon UMETA(DisplayName = "MainDungeon"),
    Boss UMETA(DisplayName = "Boss"),
    Ending UMETA(DisplayName = "Ending")
};

class USoundBase;

UCLASS()
class LINEARDUNGEON_API ALinearGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    TMap<EBGMType, USoundBase*> BGMList;

    UPROPERTY(EditDefaultsOnly, Category = "Audio")
    EBGMType DefaultBGMType = EBGMType::Opening;

    UFUNCTION(BlueprintCallable, Category = "Audio")
    void ChangeBGM(EBGMType BGMType);

};
