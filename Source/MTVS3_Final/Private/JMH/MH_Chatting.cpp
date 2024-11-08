// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_Chatting.h"
#include "Components/Button.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "HJ/TTGameInstance.h"
#include "JMH/MainWidget.h"
#include "JMH/MH_Message.h"
#include "LHM/TTPlayerController.h"

void UMH_Chatting::NativeConstruct()
{
	Super::NativeConstruct();

	OwningPlayer = GetOwningPlayer();

	// PlayerController를 가져와 캐스팅
	ChatPlayerControllerInstance = Cast<ATTPlayerController>(GetOwningPlayer());

	// SendButton 클릭 이벤트 바인딩
	if (Btn_Send)
	{
		Btn_Send->OnClicked.AddDynamic(this , &UMH_Chatting::OnClickedSendBtn);
	}
}

void UMH_Chatting::OnClickedSendBtn()
{
	if (EText_Chat && ChatPlayerControllerInstance)
	{
		//입력한 Text->String으로, 공백 제외 
		FString TrimmedText = EText_Chat->GetText().ToString().TrimStartAndEnd();

		if (!TrimmedText.IsEmpty())
		{
			//ATTPlayerController* PlayerController = Cast<ATTPlayerController>(GetWorld()->GetFirstPlayerController());
			auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
			// 앞에 UserName 붙여주기
			FString Message = FString::Printf(TEXT("%s : %s") , *gi->GetNickname() , *TrimmedText);

			// PlayerController의 서버 함수 호출
			ChatPlayerControllerInstance->ServerSendChatMessage(Message);
			
			GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("111"));
			// 인풋모드 변경
			FInputModeGameAndUI InputMode;
			ChatPlayerControllerInstance->SetInputMode(InputMode);

			// 채팅창 비우기
			EText_Chat->SetText(FText::GetEmpty());
		}
			else
			{
				UE_LOG(LogTemp , Warning , TEXT("ChatPlayerController instance or ChatInputBox is null."));
			}
		
	}
}

void UMH_Chatting::AddChatMessage(const FString& Message)
{
	// 메시지 한 줄에 해당하는 위젯을 만듦
	UMH_Message* MessageUI = CreateWidget<UMH_Message>(this , MessageWidget);
	if (MessageUI)
	{
		// Scroll box에 AddChild 부착
		MessageUI->SetChatMessage(Message);
		Scroll_Chat->AddChild(MessageUI);
		Scroll_Chat->ScrollToEnd();
		Scroll_Chat->bAnimateWheelScrolling = true;
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
