// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_TreeTicketBoxWidget.h"

#include "Components/Button.h"
#include "Components/Image.h"
#include "LHM/HM_TreeCustomTicketWidget.h"

void UHM_TreeTicketBoxWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Button_Ticket->OnClicked.AddDynamic(this, &UHM_TreeTicketBoxWidget::OnImgTicketClicked);
	
}

void UHM_TreeTicketBoxWidget::OnImgTicketClicked()
{
	// 부모 위젯의 Can_Choose 표시 및 클릭된 티켓 ID 전달
	if (auto* ParentWidget = Cast<UHM_TreeCustomTicketWidget>(GetParent()))
	{
		ParentWidget->SetCanChooseVisibility(true, TicketId);
	}
}