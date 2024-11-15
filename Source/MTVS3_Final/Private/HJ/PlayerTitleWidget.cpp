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
		bool bIsOnlyWhitespace = true;
		for (const TCHAR& Char : _TitleName)
		{
			if (!FChar::IsWhitespace(Char))
			{
				bIsOnlyWhitespace = false;
				break;
			}
		}

		FString FormattedTitleName;
		if (bIsOnlyWhitespace)
		{
			FormattedTitleName = _TitleName; // 공백만 있는 경우 그대로 표시
		}
		else 
		{
			FormattedTitleName = FString::Printf(TEXT("|%s|"), *_TitleName); // 일반 텍스트인 경우 |로 감싸기
		}

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
