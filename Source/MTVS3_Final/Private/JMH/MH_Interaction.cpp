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
		FTimerHandle RouletteTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(RouletteTimerHandle , this , &UMH_Interaction::OnAnimationFinished ,
											   0.7f , false);
	}
}

void UMH_Interaction::TextOffAnimPlay()
{
	
	//if()
	if (bAnimationFinished)
	{
		PlayAnimation(TextOffAnim);
		FTimerHandle RouletteTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(RouletteTimerHandle , this , &UMH_Interaction::OnAnimationFinished ,
											   .7f , false);

	}
}

void UMH_Interaction::OnAnimationFinished()
{
	bAnimationFinished = !bAnimationFinished;
}
