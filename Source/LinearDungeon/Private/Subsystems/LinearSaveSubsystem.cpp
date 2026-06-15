#include "Subsystems/LinearSaveSubsystem.h"
#include "Logging/StructuredLog.h"

#include "SaveGames/LinearSaveGame.h"
#include "Characters/LinearPlayerCharacter.h"
#include "Components/AttributeComponent.h"
#include "Kismet/GameplayStatics.h"

void ULinearSaveSubsystem::SaveGame(ALinearPlayerCharacter* PlayerCharacter, int32 SlotIndex)
{
	if (!PlayerCharacter) return;

    // 空のセーブデータオブジェクトを生成
    ULinearSaveGame* SaveGameObj = Cast<ULinearSaveGame>(UGameplayStatics::CreateSaveGameObject(ULinearSaveGame::StaticClass()));
    if (SaveGameObj)
    {
        // ゲーム内からデータを抽出し、セーブオブジェクトに格納
        SaveGameObj->PlayerTransform = PlayerCharacter->GetActorTransform();
        SaveGameObj->PlayerHealth = PlayerCharacter->GetAttributeComponent()->GetCurrentHealth();

        // ディスクへの書き込み実行 
        // TODO: AsyncSaveGameToSlot を使うと、セーブ時のかくつきがなくなる
        const FString SaveSlotName = FString::Printf(TEXT("%s%d"), *BaseSaveSlotName, SlotIndex); // Printf の %s に string を渡すときは、* を付与する
        UGameplayStatics::SaveGameToSlot(SaveGameObj, SaveSlotName, 0);
        UE_LOGFMT(LogTemp, Warning, "AULinearSaveSubsystem::SaveGame() SaveSlotName: {0}", SaveSlotName);
    }
}

void ULinearSaveSubsystem::LoadGame(ALinearPlayerCharacter* PlayerCharacter, int32 SlotIndex)
{
    if (!PlayerCharacter) return;

    // セーブデータが存在するか確認
    const FString SaveSlotName = FString::Printf(TEXT("%s%d"), *BaseSaveSlotName, SlotIndex);
    if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
    {
        // ディスクから読み込み
        ULinearSaveGame* LoadedGame = Cast<ULinearSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, 0));
        if (LoadedGame)
        {
            // プレイヤーにデータを反映
            PlayerCharacter->SetActorTransform(LoadedGame->PlayerTransform);
            PlayerCharacter->GetAttributeComponent()->SetCurrentHealth(LoadedGame->PlayerHealth);
            UE_LOGFMT(LogTemp, Warning, "AULinearSaveSubsystem::SaveGame() SaveSlotName: {0}", SaveSlotName);
        }
    }
}
