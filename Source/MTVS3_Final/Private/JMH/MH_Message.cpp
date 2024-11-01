// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_Message.h"

#include "Components/TextBlock.h"

void UMH_Message::SetChatMessage(const FString& Message)
{
	if(Text_Message)
	{
		Text_Message->SetText(FText::FromString(Message));
	}
}
