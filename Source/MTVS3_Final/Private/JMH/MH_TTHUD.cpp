// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_TTHUD.h"

#include "Blueprint/UserWidget.h"
#include "Components/ScrollBox.h"
#include "HJ/TTPlayer.h"
#include "JMH/MainWidget.h"
#include "JMH/MH_Chatting.h"
#include "JMH/MH_Message.h"

void AMH_TTHUD::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void AMH_TTHUD::BeginPlay()
{
	Super::BeginPlay();
	
}

void AMH_TTHUD::AddChatting()
{
	ATTPlayer* Player = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (Player)
	{
		UMainWidget* MainUI = Cast<UMainWidget>(Player->MainUI);
		UMH_Chatting* ChatUI = Cast<UMH_Chatting>(MainUI->WBP_Chatting);
		if (ChatUI)
		{
			ChatUI->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void AMH_TTHUD::AddChatMessage(const FString& Message)
{
	ATTPlayer* Player = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if (Player)
	{
		UMainWidget* MainUI = Cast<UMainWidget>(Player->MainUI);
		UMH_Chatting* ChatUI = Cast<UMH_Chatting>(MainUI->WBP_Chatting);
		
		if (ChatUI && MessageWidget)
		{
			// 메시지 한 줄에 해당하는 위젯을 만듦
			UMH_Message* MessageUI = CreateWidget<UMH_Message>(GetWorld(),MessageWidget);
			if (MessageUI)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("777"));
				// Scroll box에 AddChild 부착
				MessageUI->SetChatMessage(Message);
				ChatUI->Scroll_Chat->AddChild(MessageUI);
				ChatUI->Scroll_Chat->ScrollToEnd();
				//ChatUI->Scroll_Chat->bAnimateWheelScrolling = true;
			}
		}
	}
}
