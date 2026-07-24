#include "Subsystems/LinearFlagSubsystem.h"
#include "Logging/StructuredLog.h"

void ULinearFlagSubsystem::SetFlag(FName FlagName)
{
	if (FlagName.IsNone()) return;

	ActiveFlags.Add(FlagName);
	//UE_LOGFMT(LogTemp, Log, "ULinearFlagSubsystem::SetFlag() Added Flag: {0}", FlagName);
}

bool ULinearFlagSubsystem::CheckFlag(FName FlagName) const
{
	if (FlagName.IsNone()) return false;

	return ActiveFlags.Contains(FlagName);
}

void ULinearFlagSubsystem::RestoreAllFlags(const TSet<FName>& InSavedFlags)
{
	ActiveFlags = InSavedFlags;
	//UE_LOGFMT(LogTemp, Log, "ULinearFlagSubsystem::RestoreAllFlags() Restored {0} flags.", ActiveFlags.Num());
}

void ULinearFlagSubsystem::ResetAllEvents()
{
	ActiveFlags.Empty();
}
