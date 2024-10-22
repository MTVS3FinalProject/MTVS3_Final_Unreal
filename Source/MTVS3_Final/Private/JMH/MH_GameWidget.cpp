// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_GameWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

void UMH_GameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_BuyTicket->OnClicked.AddDynamic(this , &UMH_GameWidget::OnClickedBuyTicket);
	Btn_BuyLater->OnClicked.AddDynamic(this , &UMH_GameWidget::OnClickedBuyLater);
	Btn_Back2->OnClicked.AddDynamic(this , &UMH_GameWidget::OnClickedBuyLater);
	Btn_Back01->OnClicked.AddDynamic(this , &UMH_GameWidget::OnClickedBack01Button);
}

void UMH_GameWidget::SetWidgetSwitcher(int32 num)
{
	//서버에서 불러와서 입력
	WS_GameWidgetSwitcher->SetActiveWidgetIndex(num);
}

void UMH_GameWidget::SetTextCurrentPlayer(int32 CurrentPlayer)
{
	//남아있는 현재 인원
	//게임모드에서? 게임룰 진행되면서 가져오기
	Tex_CurrentPlayer->SetText(FText::AsNumber(CurrentPlayer));
	
}

void UMH_GameWidget::SetTextCountDown(FString CountDownTime)
{
	//남은시간
	Tex_CountDown->SetText(FText::FromString(CountDownTime));
}

void UMH_GameWidget::SetTextroulette1(FString roulette1)
{
	//룰렛1
	Tex_roulette1->SetText(FText::FromString(roulette1));
}

void UMH_GameWidget::SetTextroulette2(FString roulette2)
{
	//룰렛2
	Tex_roulette2->SetText(FText::FromString(roulette2));
}

void UMH_GameWidget::SetTextroulette3(FString roulette3)
{
	//룰렛3
	Tex_roulette3->SetText(FText::FromString(roulette3));
}

void UMH_GameWidget::SetTextSeatNum1(FString SeatNum1)
{
	//좌석 구역
	Tex_roulette3->SetText(FText::FromString(SeatNum1));
}

void UMH_GameWidget::SetTextSeatNum2(FString SeatNum2)
{
	//좌석 번호
	Tex_roulette3->SetText(FText::FromString(SeatNum2));
}

void UMH_GameWidget::OnClickedBuyTicket()
{
	//예매징행 버튼->
	//이겼다는 정보를 gi에 가지고 홀로 세션이동
	//홀세션
	//QR UI로 이동
	SetWidgetSwitcher(4);
}

void UMH_GameWidget::OnClickedBuyLater()
{
	//나중에 예매하기버튼->
	//이겼다는 정보를 gi에 가지고 홀로 세션이동
	//알림 등록하고, 알림에서 클릭시 결제 진행 가능.
}

void UMH_GameWidget::SetTextRemainingTicket(int32 RemainingTicket)
{
	//게임인스턴스에서 남아있는 티켓 잔여횟수 받아오기
	Tex_CurrentPlayer->SetText(FText::AsNumber(RemainingTicket));
}

void UMH_GameWidget::OnClickedBackButton()
{
	//콘서트 홀로 세션이동
}

void UMH_GameWidget::OnClickedBack01Button()
{
	//미니게임 -> 티켓예매창에서 뒤로가기 누르면
	//위젯 종료
}
