// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_ItemBox_Sticker.h"

#include "Components/Button.h"

void UMH_ItemBox_Sticker::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_Item_Sticker->OnClicked.AddDynamic(this,&UMH_ItemBox_Sticker::OnClickedStickerBtn);

	// 버튼이 호버될 때 델리게이트 호출
	if (UButton* Button = Cast<UButton>(GetWidgetFromName(TEXT("Btn_Item_Ticket"))))
	{
		Button->OnHovered.AddDynamic(this, &UMH_ItemBox_Sticker::OnButtonHovered_Sticker);
	}
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

void UMH_ItemBox_Sticker::SetInfoString_Sticker(FString infoString_Sticker)
{
	InfoTextString_Title = infoString_Sticker;
}

FString UMH_ItemBox_Sticker::GetInfoString_Sticker()
{
	return InfoTextString_Title;
}



