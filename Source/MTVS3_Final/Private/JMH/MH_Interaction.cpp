// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_Interaction.h"

#include "Components/WidgetSwitcher.h"

void UMH_Interaction::NativeConstruct()
{
	Super::NativeConstruct();
	
}

void UMH_Interaction::SetVisibilityInteraction(bool bISVisible, int index)
{
	if(bISVisible)
	{
		WS_InteractionWidgetSwitcher->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		WS_InteractionWidgetSwitcher->SetVisibility(ESlateVisibility::Hidden);
	}
	WS_InteractionWidgetSwitcher->SetActiveWidgetIndex(index);
}
