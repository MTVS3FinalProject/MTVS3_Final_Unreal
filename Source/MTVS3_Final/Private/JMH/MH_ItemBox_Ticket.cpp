// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_ItemBox_Ticket.h"

#include "Components/Button.h"

void UMH_ItemBox_Ticket::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_Item_Ticket->OnClicked.AddDynamic(this,&UMH_ItemBox_Ticket::OnClickedTicketBtn);

	// 버튼이 호버될 때 델리게이트 호출
	if (UButton* Button = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Item_Ticket"))))
	{
		Button->OnHovered.AddDynamic(this, &UMH_ItemBox_Ticket::OnButtonHovered_Ticket);
	}
}

//void UMH_ItemBox_Ticket::SetTicketData(const FTicketItemData& InTicketData)
//{
	
//}

void UMH_ItemBox_Ticket::OnClickedTicketBtn()
{
	
}

void UMH_ItemBox_Ticket::OnButtonHovered_Ticket()
{
	OnItemHovered_Ticket.Broadcast(true);
}
