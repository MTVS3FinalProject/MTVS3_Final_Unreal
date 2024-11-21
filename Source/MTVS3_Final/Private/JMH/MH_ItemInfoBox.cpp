// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_ItemInfoBox.h"

#include "Components/TextBlock.h"
#include "Misc/LowLevelTestAdapter.h"

void UMH_ItemInfoBox::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMH_ItemInfoBox::SetTextItemInfo(FString Info)
{
	Text_ItemInfo->SetText(FText::FromString(Info));
}

void UMH_ItemInfoBox::SetTextItemRarity(FString Rarity)
{
	ChangeColorTitleName(Rarity);
	Text_titleRarity->SetText(FText::FromString(Rarity));
}

void UMH_ItemInfoBox::PlayInfoTextAnim(bool forward)
{
	if(forward)
	{
		PlayAnimation(InfoBoxTextAnim);
	}
	
	else if (!forward)
	{
		PlayAnimation(InfoBoxTextAnim , 0 , 1 , EUMGSequencePlayMode::Reverse);
	}
}

void UMH_ItemInfoBox::ChangeColorTitleName(const FString& _TitleRarity)
{
	if (_TitleRarity == "Common")
	{
		Text_titleRarity->SetColorAndOpacity(FSlateColor(FColor(111 , 111 , 111)));
	}
	else if (_TitleRarity == "Rare")
	{
		Text_titleRarity->SetColorAndOpacity(FSlateColor(FColor(5 , 177 , 216)));
	}
	else if (_TitleRarity == "Unique")
	{
		Text_titleRarity->SetColorAndOpacity(FSlateColor(FColor(216 , 5 , 188)));
	}
	else
	{
		Text_titleRarity->SetColorAndOpacity(FSlateColor(FColor(111 , 111 , 111))); // 기본값
	}
}
