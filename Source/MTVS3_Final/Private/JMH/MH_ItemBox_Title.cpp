// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_ItemBox_Title.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UMH_ItemBox_Title::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Item_Title->OnClicked.AddDynamic(this , &UMH_ItemBox_Title::OnClickedTitle);

	// 버튼이 호버될 때 델리게이트 호출
	Btn_Item_Title->OnHovered.AddDynamic(this, &UMH_ItemBox_Title::OnButtonHovered_Title);
	Btn_Item_Title->OnUnhovered.AddDynamic(this, &UMH_ItemBox_Title::OnButtonUnHovered_Title);
	
}

void UMH_ItemBox_Title::ChangeColorTitleName(const FString& _TitleRarity)
{
	if (_TitleRarity == "Common")
	{
		Text_Title->SetColorAndOpacity(FSlateColor(FColor(111 , 111 , 111)));
	}
	else if (_TitleRarity == "Rare")
	{
		Text_Title->SetColorAndOpacity(FSlateColor(FColor(5 , 177 , 216)));
	}
	else if (_TitleRarity == "Unique")
	{
		Text_Title->SetColorAndOpacity(FSlateColor(FColor(216 , 5 , 188)));
	}
	else
	{
		Text_Title->SetColorAndOpacity(FSlateColor(FColor(111 , 111 , 111))); // 기본값
	}
}

void UMH_ItemBox_Title::SetTitleID(int32 TitleID0)
{
	TitleID = TitleID0;
}

int32 UMH_ItemBox_Title::GetTitleID()
{
	return TitleID;
}

void UMH_ItemBox_Title::SetTitleRarity(FString TitleRarity)
{
	RarityString = TitleRarity;
}

FString UMH_ItemBox_Title::GetTitleRarity()
{
	return RarityString;
}

FString UMH_ItemBox_Title::GetInfoString()
{
	return InfoTextString;
}

void UMH_ItemBox_Title::SetInfoString(FString infoString)
{
	InfoTextString = infoString;
}


//void UMH_ItemBox_Title::SetTitleData(const FTitleItemData& InTitleData)
//{


//}
/*
FReply UMH_ItemBox_Title::OnMouseButtonDoubleClick(const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("111111!"));
	Super::OnMouseButtonDoubleClick(InMyGeometry, InMouseEvent);
	// 더블 클릭 이벤트가 발생했을 때 델리게이트 호출
	OnDoubleClicked.Broadcast(this);

	return FReply::Handled();
}*/
