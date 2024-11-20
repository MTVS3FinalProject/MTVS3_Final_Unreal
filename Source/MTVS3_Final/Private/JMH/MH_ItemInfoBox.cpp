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
