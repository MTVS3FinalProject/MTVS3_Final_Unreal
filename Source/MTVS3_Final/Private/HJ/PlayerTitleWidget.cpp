// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/PlayerTitleWidget.h"
#include "Components/TextBlock.h"

void UPlayerTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UPlayerTitleWidget::UpdateTitleNameUI(const FString& _TitleName)
{
	if (!_TitleName.IsEmpty())
	{
		//FString FormattedTitleName = FString::Printf(TEXT("|%s|") , *_TitleName);
		FString FormattedTitleName = FString::Printf(TEXT("%s") , *_TitleName);
		Tex_TitleName->SetText(FText::FromString(FormattedTitleName));
	}
}

void UPlayerTitleWidget::ChangeColorTitleNameUI(const FString& _TitleRarity)
{
	if (_TitleRarity == "Common")
	{
		Tex_TitleName->SetColorAndOpacity(FSlateColor(FColor(111 , 111 , 111)));
	}
	else if (_TitleRarity == "Rare")
	{
		Tex_TitleName->SetColorAndOpacity(FSlateColor(FColor(5 , 177 , 216)));
	}
	else if (_TitleRarity == "Unique")
	{
		Tex_TitleName->SetColorAndOpacity(FSlateColor(FColor(216 , 5 , 188)));
	}
	else
	{
		Tex_TitleName->SetColorAndOpacity(FSlateColor(FColor(111 , 111 , 111))); // 기본값
	}
}
