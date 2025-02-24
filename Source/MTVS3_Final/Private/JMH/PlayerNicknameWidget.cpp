﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/PlayerNicknameWidget.h"
#include "Components/TextBlock.h"

void UPlayerNicknameWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UPlayerNicknameWidget::UpdateNicknameUI(const FString& _Nickname)
{
	Tex_Nickname->SetText(FText::FromString(_Nickname));
}


void UPlayerNicknameWidget::ChangeColorNicknameUI(const FColor& RoleColor)
{
	// 닉네임 색상 설정
	Tex_Nickname->SetColorAndOpacity(FSlateColor(RoleColor));
}