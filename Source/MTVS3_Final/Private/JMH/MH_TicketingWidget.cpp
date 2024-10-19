// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_TicketingWidget.h"

#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"

void UMH_TicketingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Btn_Back1->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedBackButton);
	Btn_Back2->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedBackButton);
	Btn_Confirm_Ticketting->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedConfirmButton);
	Btn_Cancel_Ticketting1->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedCancelButton);
	Btn_Cancel_Ticketting2->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedCancelButton);

	//위젯 꺼져있는게 기본값
	SetVisibleSwitcher(false);
	SetCompletedVisible(false);
}

void UMH_TicketingWidget::SetWidgetSwitcher(int32 num)
{
	//서버에서 불러와서 입력
	WS_RegisterSwitcher->SetActiveWidgetIndex(num);
}

void UMH_TicketingWidget::SetVisibleSwitcher(bool bVisible)
{
	if (bVisible)
	{
		WS_RegisterSwitcher->SetVisibility(ESlateVisibility::Visible);
	}

	else if (!bVisible)
	{
		WS_RegisterSwitcher->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMH_TicketingWidget::SetConcertInfo(FString ConcertName , int32 ConcertDateY , int32 ConcertDateM ,
                                         int32 ConcertDateD , FString ConcertTime)
{
	//접수화면 콘서트 정보 불러오기
	Text_ConcertName->SetText(FText::FromString(ConcertName));
	Text_ConcertDateY->SetText(FText::AsNumber(ConcertDateY));
	Text_ConcertDateM->SetText(FText::AsNumber(ConcertDateM));
	Text_ConcertDateD->SetText(FText::AsNumber(ConcertDateD));
	Text_ConcertTime->SetText(FText::FromString(ConcertTime));
}

void UMH_TicketingWidget::SetTextSeatID(int32 SeatFloor , FString SeatID)
{
	//좌석 층
	Text_SeatFloor->SetText(FText::AsNumber(SeatFloor));
	//좌석번호 세팅
	Text_SeatID->SetText(FText::FromString(SeatID));
}

void UMH_TicketingWidget::SetTickettingDate(int32 TickettingDateY , int32 TickettingDateM , int32 TickettingDateD)
{
	//좌석 추첨 날짜 정보
	Text_TickettingDateY->SetText(FText::AsNumber(TickettingDateY));
	Text_TickettingDateM->SetText(FText::AsNumber(TickettingDateM));
	Text_TickettingDateD->SetText(FText::AsNumber(TickettingDateD));
}

void UMH_TicketingWidget::SetTextGameStartTime(FString CompetitionStartTime)
{
	//좌석 추첨 시간
	//FString으로 변환해놓은 시간값 받아와서 표시
	Text_GameStartTime->SetText(FText::FromString(CompetitionStartTime));
}

void UMH_TicketingWidget::SetTextRemainingTicket(int32 RemainingTicket)
{
	//int 가격 Ftext로 변환해서 티켓가격 입력하기
	Text_RemainingTickets->SetText(FText::AsNumber(RemainingTicket));
}

void UMH_TicketingWidget::SetCompletedVisible(bool bVisible)
{
	//텍스트, 접수신청 버튼같이 꺼주기
	if (bVisible)
	{
		Ver_Completed->SetVisibility(ESlateVisibility::Visible);
		Btn_Cancel_Ticketting1->SetVisibility(ESlateVisibility::Visible);
		Btn_Confirm_Ticketting->SetVisibility(ESlateVisibility::Hidden);
	}
	
	else if (!bVisible)
	{
		Ver_Completed->SetVisibility(ESlateVisibility::Hidden);
		Btn_Cancel_Ticketting1->SetVisibility(ESlateVisibility::Hidden);
		Btn_Confirm_Ticketting->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMH_TicketingWidget::OnClickedBackButton()
{
	//뒤로가기 (위젯 숨기기 애님 플레이)
}

void UMH_TicketingWidget::OnClickedConfirmButton()
{
	//접수완료,
	//접수신청버튼 안보이게, 접수완료 text,접수취소 버튼 보이게
	SetCompletedVisible(true);
	//접수오류가 생기는 경우도 있나?
	//경고창 띄우기
}

void UMH_TicketingWidget::OnClickedCancelButton()
{
	//접수 취소는 접수 신청이 완료 되면 보여지게
	//버튼,텍스트 안보이게
	SetCompletedVisible(false);
	//서버-> 접수취소 되게
	
}

void UMH_TicketingWidget::SetTextTicketPrice(int32 TicketPrice)
{
	//int 가격 Ftext로 변환해서 티켓가격 입력하기
	Text_TicketPrice->SetText(FText::AsNumber(TicketPrice));
}

void UMH_TicketingWidget::SetSeatInfoMap(FString SeatID)
{
	//좌석별 불켜진 이미지 어떻게 만들지 이것도 이미지가 아니라
	//Widget으로 리스트 불러와서 가능한지 켜주고
	//해당 좌석에 접근햇을 때 그 좌석만 불켜주기?
	//아님 걍 이미지로 넣어두고 좌석정보에 맞춰 띄우기
}


void UMH_TicketingWidget::SetTextTicketingDeadline(FString TicketingDeadline)
{
	//FString으로 변환해놓은 시간값 받아와서 표시
	Text_TicketingDeadline->SetText(FText::FromString(TicketingDeadline));
}


void UMH_TicketingWidget::SetTextCompetitionRate(int32 CompetitionRate)
{
	//int 경쟁률 Ftext로 변환해서 경쟁률 입력하기
	Text_CompetitionRate->SetText(FText::AsNumber(CompetitionRate));
}

void UMH_TicketingWidget::OnClickedGotoGameRoomButton()
{
	//게임맵으로 세션이동
}
