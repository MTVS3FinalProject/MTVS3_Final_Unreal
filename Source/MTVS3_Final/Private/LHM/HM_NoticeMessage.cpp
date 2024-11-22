// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_NoticeMessage.h"

#include "Components/Button.h"

void UHM_NoticeMessage::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Message->OnClicked.AddDynamic(this, &UHM_NoticeMessage::HandleMessageClicked);
	
}