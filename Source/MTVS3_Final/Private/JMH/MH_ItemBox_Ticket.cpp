// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_ItemBox_Ticket.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"

void UMH_ItemBox_Ticket::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_Item_Ticket->OnClicked.AddDynamic(this,&UMH_ItemBox_Ticket::OnClickedTicketBtn);
	Btn_Item_Ticket->OnHovered.AddDynamic(this,&UMH_ItemBox_Ticket::OnButtonHovered_Ticket);
	Btn_Item_Ticket->OnUnhovered.AddDynamic(this,&UMH_ItemBox_Ticket::OnButtonUnHovered_Ticket);

	// 버튼이 호버될 때 델리게이트 호출

	HideInfo_Ticket();
}

//void UMH_ItemBox_Ticket::SetTicketData(const FTicketItemData& InTicketData)
//{
	
//}

void UMH_ItemBox_Ticket::OnClickedTicketBtn()
{
	
}

void UMH_ItemBox_Ticket::ShowInfo_Ticket()
{
	Can_Ticket->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UMH_ItemBox_Ticket::HideInfo_Ticket()
{
	Can_Ticket->SetVisibility(ESlateVisibility::Hidden);
}

void UMH_ItemBox_Ticket::SetInfoString_Ticket(FString NameString_Ticket,FString infoString_Ticket, int32 Year, int32 Month, int32 Day, FString Time)
{
	TextNameString_Ticket = NameString_Ticket;
	InfoTextString_Ticket = infoString_Ticket;
	TextYearInt_Ticket = Year;
	TextMonthInt_Ticket = Month;
	TextDayInt_Ticket = Day;
	TextTimeInt_Ticket = Time;
}