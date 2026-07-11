#include "Subsystems/LinearSaveSubsystem.h"
#include "Logging/StructuredLog.h"

#include "SaveGames/LinearSaveGame.h"
#include "Characters/LinearPlayerCharacter.h"
#include "Components/AttributeComponent.h"
#include "Interfaces/SaveInterface.h"
#include "Subsystems/LinearEventSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/LinearFlagSubsystem.h"

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

    // LevelSequence で行われる イベントフラグ情報
    if (ULinearEventSubsystem* EventSubsystem = GetGameInstance()->GetSubsystem<ULinearEventSubsystem>())
    {
        SaveGameObj->ClearedEvents = EventSubsystem->GetClearedEvents();
    }

    // 敵とのエンカウント(EncounterManager)など、FlagSubsystem で管理しているフラグ保存
    if (ULinearFlagSubsystem* FlagSubsystem = GetGameInstance()->GetSubsystem<ULinearFlagSubsystem>())
    {
        SaveGameObj->SavedFlags = FlagSubsystem->GetAllFlags();
    }

    // セーブ処理
    const FString SaveSlotName = FString::Printf(TEXT("%s%d"), *BaseSaveSlotName, SlotIndex);
    FAsyncSaveGameToSlotDelegate SaveDelegate;
    SaveDelegate.BindUObject(this, &ULinearSaveSubsystem::SaveGameCompleted);
    UGameplayStatics::AsyncSaveGameToSlot(SaveGameObj, SaveSlotName, 0, SaveDelegate);

    // ディスク書き込み
    // TODO: Async にする
    //const FString SaveSlotName = FString::Printf(TEXT("%s%d"), *BaseSaveSlotName, SlotIndex); // Printf の %s に string を渡すときは、* を付与する
    //UGameplayStatics::SaveGameToSlot(SaveGameObj, SaveSlotName, 0);
    //UE_LOGFMT(LogTemp, Warning, "AULinearSaveSubsystem::SaveGame() SaveSlotName: {0}", SaveSlotName);
}

// Save 完了時に呼ばれる関数
void ULinearSaveSubsystem::SaveGameCompleted(const FString& SlotName, const int32 UserIndex, bool bSuccess)
{
    if (bSuccess)
    {
        UE_LOGFMT(LogTemp, Warning, "ULinearSaveSubsystem::SaveGameCompleted() Success. Slot: {0}", SlotName);
        OnSaveCompleted.Broadcast();
    }
    else
    {
        UE_LOGFMT(LogTemp, Error, "ULinearSaveSubsystem::SaveGameCompleted() Failed. Slot: {0}", SlotName);
    }
}

void ULinearSaveSubsystem::LoadGame(int32 SlotIndex)
{
    // セーブデータが存在するか確認し、無ければ何もしない
    const FString SaveSlotName = FString::Printf(TEXT("%s%d"), *BaseSaveSlotName, SlotIndex);
    if (!UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0)) return;

    // 非同期読み込み
    FAsyncLoadGameFromSlotDelegate LoadDelegate;
    LoadDelegate.BindUObject(this, &ULinearSaveSubsystem::LoadGameCompleted);
    UGameplayStatics::AsyncLoadGameFromSlot(SaveSlotName, 0, LoadDelegate);

    // 通常同期読み込みのパターン
    //ULinearSaveGame* LoadedGameObj = Cast<ULinearSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
    //if (!LoadedGameObj) return;

    //// World の USaveInterface を実装している、全ての Actor を取得
    //TArray<AActor*> SaveableActors;
    //UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USaveInterface::StaticClass(), SaveableActors);

    //// ロード処理
    //// 情報を格納するのは、Actor 自身でやってもらう
    //for (AActor* Actor : SaveableActors)
    //{
    //    ISaveInterface* SaveInterface = Cast<ISaveInterface>(Actor);
    //    if (SaveInterface)
    //    {
    //        SaveInterface->OnLoadGame(LoadedGameObj);
    //    }
    //}
    //UE_LOGFMT(LogTemp, Warning, "ULinearSaveSubsystem::LoadGame() Complete. Slot: {0}", SaveSlotName);

}


void ULinearSaveSubsystem::LoadGameCompleted(const FString& SlotName, const int32 UserIndex, USaveGame* LoadedGameData)
{
    ULinearSaveGame* LoadedGameObj = Cast<ULinearSaveGame>(LoadedGameData);
    if (!LoadedGameObj) return;

    // Level Sequence などイベントフラグ復元
    if (ULinearEventSubsystem* EventSubsystem = GetGameInstance()->GetSubsystem<ULinearEventSubsystem>())
    {
        EventSubsystem->SetClearedEvents(LoadedGameObj->ClearedEvents);
    }

    // EncounterManager などの進行フラグ復元
    if (ULinearFlagSubsystem* FlagSubsystem = GetGameInstance()->GetSubsystem<ULinearFlagSubsystem>())
    {
        FlagSubsystem->RestoreAllFlags(LoadedGameObj->SavedFlags);
    }

    // 各 Actor の個別ロード処理
    TArray<AActor*> SaveableActors;
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), USaveInterface::StaticClass(), SaveableActors);

    for (AActor* Actor : SaveableActors)
    {
        ISaveInterface* SaveInterface = Cast<ISaveInterface>(Actor);
        if (SaveInterface)
        {
            SaveInterface->OnLoadGame(LoadedGameObj);
        }
    }

    UE_LOGFMT(LogTemp, Warning, "ULinearSaveSubsystem::LoadGameCompleted() Applied Data. Slot: {0}", SlotName);
}

// PlayerController.RestartGame() 等で使う
// 死亡 | 通常ロード時、引数で渡したスロットでロードするようにこの関数を読んで予約する
void ULinearSaveSubsystem::SetPendingLoad(int32 SlotIndex)
{
    PendingLoadSlotIndex = SlotIndex;
    bHasPendingLoad = true;
    UE_LOGFMT(LogTemp, Warning, "ULinearSaveSubsystem::SetPendingLoad() Reserved Slot: {0}", SlotIndex);
}

// LinearGameMode.BeginPlay() 等で使う
// 予約済みフラグが true なら、渡された OutSlotIndex を、予約済みスロットの数値で書き換え(&で参照渡しになっているので）
// その後、BeginPlay() などから SaveSubsystem->LoadGame(SlotToLoad); を呼べば、予約済みスロットをロードできる
// 終わった後はフラグを消しておく
bool ULinearSaveSubsystem::ConsumePendingLoad(int32& OutSlotIndex)
{
    if (bHasPendingLoad)
    {
        OutSlotIndex = PendingLoadSlotIndex;
        bHasPendingLoad = false;
        return true;
    }
    return false;
}
