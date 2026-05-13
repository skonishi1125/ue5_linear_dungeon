#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LinearCharacterBase.generated.h"

UCLASS()
class LINEARDUNGEON_API ALinearCharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ALinearCharacterBase();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

};
