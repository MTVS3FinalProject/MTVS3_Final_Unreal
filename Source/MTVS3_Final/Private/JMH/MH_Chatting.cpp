// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_Chatting.h"

#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "GameFramework/PlayerState.h"
#include "HJ/TTPlayer.h"
#include "JMH/MainWidget.h"
#include "JMH/MH_ChatManager.h"
#include "JMH/MH_Message.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/TTPlayerController.h"

void UMH_Chatting::NativeConstruct()
{
	Super::NativeConstruct();
	
	OwningPlayer = GetOwningPlayer();

	Btn_Send->OnClicked.AddDynamic(this , &UMH_Chatting::UMH_Chatting::OnClickedSendBtn);
}

void UMH_Chatting::OnClickedSendBtn()
{
	if (EText_Chat)
	{
		FString TrimmedText = EText_Chat->GetText().ToString().TrimStartAndEnd();

		if (!TrimmedText.IsEmpty())
		{
			ATTPlayerController* PlayerController = Cast<ATTPlayerController>(GetWorld()->GetFirstPlayerController());
			if (PlayerController)
			{
				// 앞에 UserName 붙여주기
				APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>();
				FString Message = FString::Printf(TEXT("%s : %s") , *PlayerState->GetPlayerName() , *TrimmedText);

				// PlayerController를 통해 서버로 메시지 전송
				PlayerController->ServerSendChatMessage(Message);
				GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("111"));
				// 인풋모드 변경
				FInputModeGameAndUI InputMode;
				PlayerController->SetInputMode(InputMode);

				// 채팅창 비우기
				EText_Chat->SetText(FText::GetEmpty());
			}
		}
	}
}

void UMH_Chatting::AddChatMessage(const FString& Message)
{
	// 메시지 한 줄에 해당하는 위젯을 만듦
	UMH_Message* MessageUI = CreateWidget<UMH_Message>(this,MessageWidget);
	if (MessageUI)
	{
		GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("777"));
		// Scroll box에 AddChild 부착
		MessageUI->SetChatMessage(Message);
		Scroll_Chat->AddChild(MessageUI);
		Scroll_Chat->ScrollToEnd();
		//ChatUI->Scroll_Chat->bAnimateWheelScrolling = true;
	}
}

void UMH_Chatting::ActivateChatText()
{
	if (EText_Chat)
	{
		EText_Chat->SetIsEnabled(true);
		EText_Chat->SetFocus();
	}
}
