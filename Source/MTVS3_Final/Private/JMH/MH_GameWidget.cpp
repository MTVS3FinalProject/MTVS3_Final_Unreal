// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_GameWidget.h"

#include "Components/WidgetSwitcher.h"

void UMH_GameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
}

void UMH_GameWidget::SetWidgetSwitcher(int32 num)
{
	//서버에서 불러와서 입력
	WS_GameWidgetSwitcher->SetActiveWidgetIndex(num);
}

void UMH_GameWidget::SetTextroulette1(FString roulette1)
{
	
}
