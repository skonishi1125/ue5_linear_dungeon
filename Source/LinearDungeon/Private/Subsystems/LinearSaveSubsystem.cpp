#include "Subsystems/LinearSaveSubsystem.h"
#include "Logging/StructuredLog.h"

#include "SaveGames/LinearSaveGame.h"
#include "Characters/LinearPlayerCharacter.h"
#include "Components/AttributeComponent.h"
#include "Interfaces/SaveInterface.h"
#include "Kismet/GameplayStatics.h"

void ULinearSaveSubsystem::SaveGame(int32 SlotIndex)
{
    // セーブする情報を格納する、空のセーブデータオブジェクトを生成
    ULinearSaveGame* SaveGameObj = Cast<ULinearSaveGame>(UGameplayStatics::CreateSaveGameObject(ULinearSaveGame::StaticClass()));
    if (!SaveGameObj) return;

    // タイムスタンプ記入
    SaveGameObj->SaveDate = FDateTime::Now();

    // World の USaveInterface 実装済 Actor を取得
    TArray<AActor*> SaveableActors;
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USaveInterface::StaticClass(), SaveableActors);

    // Actor の Interfaces 処理を読んで、SaveGameObj に記入させる
    for (AActor* Actor : SaveableActors)
    {
        ISaveInterface* SaveInterface = Cast<ISaveInterface>(Actor);
        if (SaveInterface)
        {
            SaveInterface->OnSaveGame(SaveGameObj);
        }
    }

    // ディスク書き込み
    // TODO: Async にする
    const FString SaveSlotName = FString::Printf(TEXT("%s%d"), *BaseSaveSlotName, SlotIndex); // Printf の %s に string を渡すときは、* を付与する
    UGameplayStatics::SaveGameToSlot(SaveGameObj, SaveSlotName, 0);
    UE_LOGFMT(LogTemp, Warning, "AULinearSaveSubsystem::SaveGame() SaveSlotName: {0}", SaveSlotName);
}

void ULinearSaveSubsystem::LoadGame(int32 SlotIndex)
{
    // セーブデータが存在するか確認し、無ければ何もしない
    const FString SaveSlotName = FString::Printf(TEXT("%s%d"), *BaseSaveSlotName, SlotIndex);
    if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0)) return;

    // ディスクからデータを読み込む
    ULinearSaveGame* LoadedGameObj = Cast<ULinearSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
    if (!LoadedGameObj) return;

    // World の USaveInterface を実装している、全ての Actor を取得
    TArray<AActor*> SaveableActors;
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USaveInterface::StaticClass(), SaveableActors);

    // ロード処理
    // 情報を格納するのは、Actor 自身でやってもらう
    for (AActor* Actor : SaveableActors)
    {
        ISaveInterface* SaveInterface = Cast<ISaveInterface>(Actor);
        if (SaveInterface)
        {
            SaveInterface->OnLoadGame(LoadedGameObj);
        }
    }
    UE_LOGFMT(LogTemp, Warning, "ULinearSaveSubsystem::LoadGame() Complete. Slot: {0}", SaveSlotName);
}
