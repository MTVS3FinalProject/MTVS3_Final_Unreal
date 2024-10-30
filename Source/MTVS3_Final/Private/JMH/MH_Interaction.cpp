// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_Interaction.h"
#include "Components/WidgetSwitcher.h"

void UMH_Interaction::NativeConstruct()
{
	Super::NativeConstruct();
	
} 

void UMH_Interaction::SetActiveWidgetIndex(int index)
{
	WS_InteractionWidgetSwitcher->SetActiveWidgetIndex(index);
}
