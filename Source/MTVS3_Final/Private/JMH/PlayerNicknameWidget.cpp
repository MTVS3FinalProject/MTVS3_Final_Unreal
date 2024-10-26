// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/PlayerNicknameWidget.h"
#include "Components/TextBlock.h"
#include "HJ/TTGameInstance.h"
#include "HJ/TTPlayerState.h"
#include "Kismet/GameplayStatics.h"

void UPlayerNicknameWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPlayerNicknameWidget::UpdateNicknameUI(const FString& _Nickname)
{
	if (Tex_Nickname)
	{
		Tex_Nickname->SetText(FText::FromString(_Nickname));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Tex_Nickname is nullptr in UpdateNicknameUI"));
	}
}


void UPlayerNicknameWidget::ChangeColorNicknameUI()
{
	// 기본 색상 설정
	FColor RoleColor = FColor::Yellow;

	// 닉네임 색상 설정
	Tex_Nickname->SetColorAndOpacity(FSlateColor(RoleColor));
}