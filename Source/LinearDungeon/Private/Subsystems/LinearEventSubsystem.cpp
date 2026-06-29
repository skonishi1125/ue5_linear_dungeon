#include "Subsystems/LinearEventSubsystem.h"
#include "Logging/StructuredLog.h"

bool ULinearEventSubsystem::IsEventCleared(FName EventID) const
{
	return ClearedEvents.Contains(EventID);
}

void ULinearEventSubsystem::MarkEventAsCleared(FName EventID)
{
	if (!ClearedEvents.Contains(EventID))
	{
		ClearedEvents.Add(EventID);
	}

	UE_LOGFMT(LogTemp, Warning, "ULinearEventSubsystem::MarkEventAsCleared(). Completed Event: {0}", EventID);

}

void ULinearEventSubsystem::ResetAllEvents()
{
	ClearedEvents.Empty();
}
