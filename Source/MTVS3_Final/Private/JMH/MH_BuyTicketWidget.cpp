// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_BuyTicketWidget.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

void UMH_BuyTicketWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Confirm01->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm01Button);
}

void UMH_BuyTicketWidget::SetWidgetSwitcher(int32 num)
{
		//서버에서 불러와서 입력
		WS_BuyTicketSwitcher->SetActiveWidgetIndex(num);
}

void UMH_BuyTicketWidget::OnClickedConfirm01Button()
{
	SetWidgetSwitcher(1);
}

void UMH_BuyTicketWidget::OnClickedBack01Button()
{
	//뒤로가기 하면 어디로?
}

//배송지 입력에서 확인버튼
void UMH_BuyTicketWidget::OnClickedConfirm02Button()
{
	//이름, 번호, 주소 서버로 보내기
}

void UMH_BuyTicketWidget::OnClickedBack02Button()
{
	//0 : 티켓 예매정보 확인으로
	SetWidgetSwitcher(0);
}

void UMH_BuyTicketWidget::OnClickedConfirm_QRUi1Button()
{
	//QR 인증 확인 요청
	
}

void UMH_BuyTicketWidget::OnClickedBack_QRUi1Button()
{
	//1 : 배송지 입력으로
	
}

