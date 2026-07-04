#pragma once

// UE における Enum の作法として、E...という形で定義する
UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon"),
};

UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_Defensing UMETA(DisplayName = "Defensing"),
	EAS_Rolling UMETA(DisplayName = "Rolling"),
	EAS_Hitting UMETA(DisplayName = "Hitting"), // 被弾
	EAS_Dying UMETA(DisplayName = "Dying"),
	EAS_InDialogue UMETA(DisplayName = "InDialogue"), // 会話中
	EAS_UsingItem UMETA(DisplayName = "UsingItem")

};

// Enemy にも Character どちらも使う
UENUM(BlueprintType)
enum class EDeathPose : uint8
{
	EDP_Alive UMETA(DisplayName = "Alive"),
	EDP_Death1 UMETA(DisplayName = "Death1"),
	EDP_Death2 UMETA(DisplayName = "Death2"),
};

UENUM(BlueprintType)
enum class EDeathCause : uint8
{
	EDC_Normal UMETA(DisplayName = "Normal Hit"),
	EDC_Fall UMETA(DisplayName = "Fall")
};

UENUM(BlueprintType)
enum class EAttackCollisionType : uint8
{
	EAC_RightHand UMETA(DisplayName = "Right Hand"),
	EAC_LeftHand UMETA(DisplayName = "Left Hand"),
	EAC_RightLeg UMETA(DisplayName = "Right Leg"),
	EAC_LeftLeg UMETA(DisplayName = "Left Leg"),
	EAC_BothHands UMETA(DisplayName = "Both Hands"),
	EAC_BothLegs UMETA(DisplayName = "Both Legs")
};
