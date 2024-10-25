// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_GameStartCountDown.h"

#include "Components/TextBlock.h"

void UMH_GameStartCountDown::NativeConstruct()
{
	Super::NativeConstruct();
	
	CountdownValue = 5;
	StartCountdown();
}

void UMH_GameStartCountDown::StartCountdown()
{
	GetWorld()->GetTimerManager().SetTimer(CountdownTimerHandle, this, &UMH_GameStartCountDown::UpdateCountdown, 1.0f, true);
}

void UMH_GameStartCountDown::UpdateCountdown()
{
	if (Tex_CountDown)
	{
		Tex_CountDown->SetText(FText::FromString(FString::FromInt(CountdownValue)));
		PlayAnimation(CountDownAnim);
	}
	
	CountdownValue--;
	
	if (CountdownValue < 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(CountdownTimerHandle);
	}
}
