// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_ItemBox_Sticker.h"

#include "Components/Button.h"

void UMH_ItemBox_Sticker::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_Item_Sticker->OnClicked.AddDynamic(this,&UMH_ItemBox_Sticker::OnClickedStickerBtn);
	
}

void UMH_ItemBox_Sticker::SetStickerData(int32 StickerID, FString StickerScript, UTexture2D* StickerImg)
{
	//티켓 아이디, 이미지,
	//티켓 아이디에서 티켓 정보 String 가져와서 info창에 띄우기
}

void UMH_ItemBox_Sticker::OnClickedStickerBtn()
{
	
}



