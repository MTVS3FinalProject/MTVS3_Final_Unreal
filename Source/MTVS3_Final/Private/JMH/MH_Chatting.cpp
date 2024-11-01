// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_Chatting.h"

#include "Components/EditableText.h"
#include "GameFramework/PlayerState.h"
#include "Interfaces/OnlinePresenceInterface.h"
#include "JMH/MH_ChatManager.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/TTPlayerController.h"

void UMH_Chatting::NativeConstruct()
{
	Super::NativeConstruct();

	//Text Commit 콜백 함수 바인딩
	if(EText_Chat)
	{
		EText_Chat->OnTextCommitted.AddDynamic(this, &UMH_Chatting::OnTextCommitted);
	}

	ChatManager = Cast<AMH_ChatManager>(UGameplayStatics::GetActorOfClass(GetWorld(),AMH_ChatManager::StaticClass()));
}

void UMH_Chatting::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{

	if(CommitMethod == ETextCommit::OnEnter)
	{
		if(EText_Chat)
		{
			//좌우 공백 제거
			FText InputText = EText_Chat->GetText();
			FString TrimmedText = InputText.ToString().TrimStartAndEnd();

			if(!TrimmedText.IsEmpty())
			{
				ATTPlayerController* PlayerController = Cast<ATTPlayerController>(GetWorld()->GetFirstPlayerController());
				if(PlayerController)
				{
					//앞에 UserName 붙여주기
					APlayerState* PlayerState = PlayerController->GetPlayerState<APlayerState>();
					FString Message = FString::Printf(TEXT("%s : %s"), *PlayerState->GetPlayerName(), *TrimmedText);
					
					//채팅을 보내기 위해 호출
					ChatManager->ServerSendChatMessage(Message);

					//인풋모드 변경
					FInputModeGameAndUI InputMode;
					PlayerController->SetInputMode(InputMode);

					//채팅창 비우고 비활성화
					EText_Chat->SetText(FText::GetEmpty());
					EText_Chat->SetIsEnabled(false);
					
				}
			}
		}
	}
}

void UMH_Chatting::ActivateChatText()
{
	if(EText_Chat)
	{
		EText_Chat->SetIsEnabled(true);
		EText_Chat->SetFocus();
	}
}
