// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_ErrorMessage.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UMH_ErrorMessage::NativeConstruct()
{
	Super::NativeConstruct();

	//델리게이트로 메인메뉴와 연결
	//Btn_ErrorExit->OnClicked.AddDynamic(this,&UMH_ErrorMessage::CloseBtnPressedErrorExit);
}

void UMH_ErrorMessage::SetTextMessage(FString ErrorMS)
{
	Text_ErrorMessage->SetText(FText::FromString(ErrorMS));
}

void UMH_ErrorMessage::ShowErrorMessage(FString ErrorMS)
{
	SetTextMessage(ErrorMS);
}


