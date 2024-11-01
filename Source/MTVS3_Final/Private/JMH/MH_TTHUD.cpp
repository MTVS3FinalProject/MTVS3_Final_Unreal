// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_TTHUD.h"

#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "JMH/MH_Chatting.h"
#include "JMH/MH_Message.h"

void AMH_TTHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && ChatWidget)
	{
		ChatUI = CreateWidget<UMH_Chatting>(PlayerController,ChatWidget);
		if (ChatUI)
		{
			ChatUI->AddToViewport();
		}
	}
}

void AMH_TTHUD::AddChatting()
{
	if (ChatUI)
	{
		ChatUI->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMH_TTHUD::AddChatMessage(const FString& Message)
{
	if (ChatUI && MessageWidget)
	{
		// 메시지 한 줄에 해당하는 위젯을 만듦
		UMH_Message* ChatMessageWidget = CreateWidget<UMH_Message>(OwningPlayer, MessageWidget);
		if (ChatMessageWidget)
		{
			// Scroll box에 AddChild 부착
			ChatMessageWidget->SetChatMessage(Message);
			ChatUI->Scroll_Chat->AddChild(ChatMessageWidget);
			ChatUI->Scroll_Chat->ScrollToEnd();
			ChatUI->Scroll_Chat->bAnimateWheelScrolling = true;
		}
	}
}
