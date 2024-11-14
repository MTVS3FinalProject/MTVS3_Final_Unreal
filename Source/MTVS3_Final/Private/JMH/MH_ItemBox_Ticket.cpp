// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_ItemBox_Ticket.h"

#include "Components/Button.h"

void UMH_ItemBox_Ticket::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_Item_Ticket->OnClicked.AddDynamic(this,&UMH_ItemBox_Ticket::OnClickedTicketBtn);
}

//void UMH_ItemBox_Ticket::SetTicketData(const FTicketItemData& InTicketData)
//{
	
//}

void UMH_ItemBox_Ticket::OnClickedTicketBtn()
{
	
}

void UMH_ItemBox_Ticket::OnButtonHovered_Ticket()
{
	
}
