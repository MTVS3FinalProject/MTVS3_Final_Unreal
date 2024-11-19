// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_Interaction.h"
#include "Components/WidgetSwitcher.h"

void UMH_Interaction::NativeConstruct()
{
	Super::NativeConstruct();

	// Delegate 바인딩
	FWidgetAnimationDynamicEvent AnimationDelegate;
	AnimationDelegate.BindDynamic(this , &UMH_Interaction::OnAnimationFinished);
	BindToAnimationFinished(TextOnAnim , AnimationDelegate);
	BindToAnimationFinished(TextOffAnim , AnimationDelegate);
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
	}
}

void UMH_Interaction::TextOffAnimPlay()
{
	if (bAnimationFinished)
	{
		PlayAnimation(TextOffAnim);
	}
}

void UMH_Interaction::OnAnimationFinished()
{
	bAnimationFinished = !bAnimationFinished;
}
