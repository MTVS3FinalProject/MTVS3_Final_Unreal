// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_BuyTicketWidget.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

void UMH_BuyTicketWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Confirm1->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm1Button);
}

void UMH_BuyTicketWidget::SetWidgetSwitcher(int32 num)
{
		//서버에서 불러와서 입력
		WS_BuyTicketSwitcher->SetActiveWidgetIndex(num);
}

void UMH_BuyTicketWidget::OnClickedConfirm1Button()
{
	SetWidgetSwitcher(1);
}

void UMH_BuyTicketWidget::OnClickedBack01Button()
{
	//뒤로가기 하면 어디로?
}

