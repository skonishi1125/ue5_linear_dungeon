#include "Components/HUD/InteractMarker.h"
#include "Components/TextBlock.h"

void UInteractMarker::SetInteractText(FText Text)
{
	if (InteractText)
	{
		InteractText->SetText(Text);
	}
}
