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

void UMH_Interaction::TextOnAnimPlay()
{
	if (!bAnimationFinished)
	{
		PlayAnimation(TextOnAnim);
		OnAnimationFinished();
	}
}

void UMH_Interaction::TextOffAnimPlay()
{
	if (bAnimationFinished)
	{
		PlayAnimation(TextOffAnim);
		OnAnimationFinished();
	}
}

void UMH_Interaction::OnAnimationFinished()
{
	bAnimationFinished = !bAnimationFinished;
}
