// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_GameStartCountDown.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"

void UMH_GameStartCountDown::NativeConstruct()
{
	Super::NativeConstruct();
	
	CountdownValue = 5;
	//StartCountdown();
	//Text_CountDown->SetVisibility(ESlateVisibility::Hidden);
}

void UMH_GameStartCountDown::StartCountdown()
{
	//Text_CountDown->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &UMH_GameStartCountDown::UpdateCountdown, 1.0f, true);
}

void UMH_GameStartCountDown::UpdateCountdown()
{
	//Text_CountDown->SetVisibility(ESlateVisibility::Visible);
	CountdownValue--;
	if (Text_CountDown)
	{
		Text_CountDown->SetText(FText::FromString(FString::FromInt(CountdownValue)));
		PlayAnimation(CountDownAnim);
	}
	
	
	if (CountdownValue < 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
	}
}

/*
void UMH_GameStartCountDown::StartCountDownVisible(bool bIsvisible)
{
	if(bIsvisible)
	{
		Can_StartCountDown->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Can_StartCountDown->SetVisibility(ESlateVisibility::Hidden);
	}
}*/
