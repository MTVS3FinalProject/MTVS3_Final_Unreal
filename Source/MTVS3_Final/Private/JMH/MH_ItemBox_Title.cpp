// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_ItemBox_Title.h"

#include "Components/Button.h"

void UMH_ItemBox_Title::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Item_Title->OnClicked.AddDynamic(this,&UMH_ItemBox_Title::OnClickedTitleBtn);
	
}

void UMH_ItemBox_Title::SetTitleData(int32 TitleID, FString TitleName, UTexture2D* TitleImg)
{


	
}

void UMH_ItemBox_Title::OnClickedTitleBtn()
{
	
}
