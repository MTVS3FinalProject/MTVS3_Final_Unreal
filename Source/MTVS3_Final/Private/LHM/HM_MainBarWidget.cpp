// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_MainBarWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

void UHM_MainBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Emoji->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedEmojiBtn);
	Btn_CollectionBook->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedCollectionBookBtn);
	Btn_Notice->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedNoticeBtn);
	
	SetVisibleSwitcher(false);
}

void UHM_MainBarWidget::SetVisibleSwitcher(bool bVisible)
{
	if ( bVisible )
	{
		WS_Bar->SetVisibility(ESlateVisibility::Visible);
	}

	else if ( !bVisible )
	{
		WS_Bar->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UHM_MainBarWidget::SetWidgetSwitcher(int32 num)
{
	WS_Bar->SetActiveWidgetIndex(num);
}

void UHM_MainBarWidget::OnClickedEmojiBtn()
{
	bIsEmojiiVisible = !bIsEmojiiVisible;

	if ( bIsEmojiiVisible )
	{
		SetWidgetSwitcher(0);
		SetVisibleSwitcher(true);
	}
	else
	{
		SetVisibleSwitcher(false);
	}
}

void UHM_MainBarWidget::OnClickedCollectionBookBtn()
{
	bIsCollectionBookVisible = !bIsCollectionBookVisible;

	if ( bIsCollectionBookVisible )
	{
		SetWidgetSwitcher(1);
		SetVisibleSwitcher(true);
	}
	else
	{
		SetVisibleSwitcher(false);
	}
}

void UHM_MainBarWidget::OnClickedNoticeBtn()
{
	bIsNoticeVisible = !bIsNoticeVisible;

	if ( bIsNoticeVisible )
	{
		SetWidgetSwitcher(2);
		SetVisibleSwitcher(true);
	}
	else
	{
		SetVisibleSwitcher(false);
	}
}
