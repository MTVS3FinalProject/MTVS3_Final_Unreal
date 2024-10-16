// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MainWidget.h"

#include "Components/CanvasPanel.h"

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	SetVisibleCanvas(true);
}

void UMainWidget::SetVisibleCanvas(bool bVisible)
{
	if (bVisible)
	{
		Can_Main->SetVisibility(ESlateVisibility::Visible);
	}
	
	else if(!bVisible)
	{
		Can_Main->SetVisibility(ESlateVisibility::Hidden);
	}
}
