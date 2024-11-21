// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_ItemBox_Sticker.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"

void UMH_ItemBox_Sticker::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_Item_Sticker->OnClicked.AddDynamic(this,&UMH_ItemBox_Sticker::OnClickedStickerBtn);

	// 버튼이 호버될 때 델리게이트 호출
	
	Btn_Item_Sticker->OnHovered.AddDynamic(this, &UMH_ItemBox_Sticker::OnButtonHovered_Sticker);
	Btn_Item_Sticker->OnUnhovered.AddDynamic(this, &UMH_ItemBox_Sticker::OnButtonUnHovered_Sticker);

	//Info창 꺼져있게
	HideInfo_Sticker();
}

//void UMH_ItemBox_Sticker::SetStickerData(const FStickerItemData& InStickerData)
//{
	//티켓 아이디, 이미지,
	//티켓 아이디에서 티켓 정보 String 가져와서 info창에 띄우기
//}

void UMH_ItemBox_Sticker::OnClickedStickerBtn()
{
	//?
}

void UMH_ItemBox_Sticker::ChangeColorStickerName(const FString& StickerRarity)
{
	if (StickerRarity == "Common")
	{
		Text_Sticker->SetColorAndOpacity(FSlateColor(FColor(111 , 111 , 111)));
	}
	else if (StickerRarity == "Rare")
	{
		Text_Sticker->SetColorAndOpacity(FSlateColor(FColor(5 , 177 , 216)));
	}
	else if (StickerRarity == "Unique")
	{
		Text_Sticker->SetColorAndOpacity(FSlateColor(FColor(216 , 5 , 188)));
	}
	else
	{
		Text_Sticker->SetColorAndOpacity(FSlateColor(FColor(111 , 111 , 111))); // 기본값
	}
}

void UMH_ItemBox_Sticker::SetInfoString_Sticker(FString infoString_Sticker)
{
	InfoTextString_Sticker = infoString_Sticker;
}

FString UMH_ItemBox_Sticker::GetInfoString_Sticker()
{
	return InfoTextString_Sticker;
}

void UMH_ItemBox_Sticker::ShowInfo_Sticker()
{
	Can_StickerInfo->SetVisibility(ESlateVisibility::HitTestInvisible);
}

void UMH_ItemBox_Sticker::HideInfo_Sticker()
{
	Can_StickerInfo->SetVisibility(ESlateVisibility::Hidden);
}

void UMH_ItemBox_Sticker::SetStickerRarity(FString StickerRarity)
{
	RarityString_Sticker = StickerRarity;
}

FString UMH_ItemBox_Sticker::GetStickerRarity()
{
	return RarityString_Sticker;
}


