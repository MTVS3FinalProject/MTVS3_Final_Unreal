// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_TicketingWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

void UMH_TicketingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Back1->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedBackButton);
	Btn_Back2->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedBackButton);
	Btn_Confirm_Ticketting->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedConfirmButton);
	Btn_Cancel_Ticketting1->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedCancelButton);
	Btn_Cancel_Ticketting2->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedCancelButton);
}

void UMH_TicketingWidget::SetWidgetSwitcher(int32 num)
{
	//서버에서 불러와서 입력
	WS_RegisterSwitcher->SetActiveWidgetIndex(num);
}

void UMH_TicketingWidget::OnClickedBackButton()
{
	//뒤로가기 (위젯 숨기기 애님)
	
}

void UMH_TicketingWidget::OnClickedConfirmButton()
{
	//접수완료, // 접수오류가 생기는 경우도 있나?
	//접수취소 버튼 보이게
}

void UMH_TicketingWidget::OnClickedCancelButton()
{
	//접수 취소는 접수 신청이 완료 되면 보여지게
	//접수취소 되게
}

void UMH_TicketingWidget::SetTextTicketPrice(int32 TicketPrice)
{
	//int 가격 Ftext로 변환해서 티켓가격 입력하기
	Tex_TicketPrice->SetText(FText::AsNumber(TicketPrice));
}

void UMH_TicketingWidget::SetTextRemainingTicket(int32 RemainingTicket)
{
	//int 가격 Ftext로 변환해서 티켓가격 입력하기
	Tex_RemainingTicket->SetText(FText::AsNumber(RemainingTicket));
}

void UMH_TicketingWidget::SetTextTicketingDeadline(FString TicketingDeadline)
{
	//FString으로 변환해놓은 시간값 받아와서 표시
	Tex_TicketingDeadline->SetText(FText::FromString(TicketingDeadline));
}

void UMH_TicketingWidget::SetTextGameStartTime(FString CompetitionStartTime)
{
	//FString으로 변환해놓은 시간값 받아와서 표시
	Tex_GameStartTime->SetText(FText::FromString(CompetitionStartTime));
}

void UMH_TicketingWidget::SetTextCompetitionRate(int32 CompetitionRate)
{
	//int 경쟁률 Ftext로 변환해서 경쟁률 입력하기
	Tex_CompetitionRate->SetText(FText::AsNumber(CompetitionRate));
}

void UMH_TicketingWidget::SetTextSeatID(FString SeatID)
{
	//좌석번호 세팅
	//FString으로 변환해놓은 시간값 받아와서 표시
	Tex_GroupSeatInfoText->SetText(FText::FromString(SeatID));
}

void UMH_TicketingWidget::OnClickedGotoGameRoomButton()
{
	//게임맵으로 세션이동
	
}


