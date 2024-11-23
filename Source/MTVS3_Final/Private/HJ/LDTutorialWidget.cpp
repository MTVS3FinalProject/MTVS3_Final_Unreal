// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/LDTutorialWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/WidgetSwitcher.h"

void ULDTutorialWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Can_LDTutorial->SetVisibility(ESlateVisibility::Visible);
	SetWidgetSwitcher(0);
	
	Btn_TutorialStart->OnClicked.AddDynamic(this , &ULDTutorialWidget::OnClickedTutorialStart);
	Btn_TutorialSkip->OnClicked.AddDynamic(this , &ULDTutorialWidget::OnClickedTutorialSkip);
	Btn_Right1->OnClicked.AddDynamic(this , &ULDTutorialWidget::OnClickedRight1);
	Btn_TutorialEnd->OnClicked.AddDynamic(this , &ULDTutorialWidget::OnClickedTutorialEnd);
}

void ULDTutorialWidget::SetWidgetSwitcher(int32 num)
{
	WS_Tutorial->SetActiveWidgetIndex(num);
}

void ULDTutorialWidget::OnClickedTutorialStart()
{
	SetWidgetSwitcher(1);
}

void ULDTutorialWidget::OnClickedTutorialSkip()
{
	Can_LDTutorial->SetVisibility(ESlateVisibility::Hidden);
}

void ULDTutorialWidget::OnClickedRight1()
{
	SetWidgetSwitcher(2);
}

void ULDTutorialWidget::OnClickedTutorialEnd()
{
	Can_LDTutorial->SetVisibility(ESlateVisibility::Hidden);
}
