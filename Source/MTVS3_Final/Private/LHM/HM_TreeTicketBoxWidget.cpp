// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_TreeTicketBoxWidget.h"

#include "Components/Button.h"
#include "LHM/HM_TreeCustomTicketWidget.h"

void UHM_TreeTicketBoxWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_Ticket->OnClicked.AddDynamic(this, &UHM_TreeTicketBoxWidget::HandleTicketClicked);
}
