// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_MainBarWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/WidgetSwitcher.h"

void UHM_MainBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Emoji->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedEmojiBtn);
	Btn_CollectionBook->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedCollectionBookBtn);
	Btn_Notice->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedNoticeBtn);
	Btn_lightMode->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedlightModeBtn);
	Btn_DarkMode->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedDarkModeBtn);
	Btn_Menu->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedlMenuBtn);
	Btn_Chat->OnClicked.AddDynamic(this , &UHM_MainBarWidget::CloseButtonPressed);
	Btn_Setting->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedSettingBtn);
	Btn_Back_Settings->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedSettingBackBtn);

	SetVisibleSwitcher(false);
}

void UHM_MainBarWidget::SetVisibleSwitcher(bool bVisible)
{
	if (bVisible)
	{
		WS_Bar->SetVisibility(ESlateVisibility::Visible);
	}

	else if (!bVisible)
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
	bIsEmojiVisible = !bIsEmojiVisible;

	if (bIsEmojiVisible)
	{
		SetWidgetSwitcher(0);
		SetVisibleSwitcher(true);
		if (bIsMenuVisible)
		{
			OnClickedlMenuBtn();
		}
		if(bIsChatVisible)
		{
			CloseButtonPressed();
		}
	}
	else
	{
		SetVisibleSwitcher(false);
	}
}

void UHM_MainBarWidget::OnClickedCollectionBookBtn()
{
	bIsCollectionBookVisible = !bIsCollectionBookVisible;

	if (bIsCollectionBookVisible)
	{
		SetWidgetSwitcher(1);
		SetVisibleSwitcher(true);
		if (bIsMenuVisible)
		{
			OnClickedlMenuBtn();
		}
		if(bIsChatVisible)
		{
			CloseButtonPressed();
		}
	}
	else
	{
		SetVisibleSwitcher(false);
	}
}

void UHM_MainBarWidget::OnClickedNoticeBtn()
{
	bIsNoticeVisible = !bIsNoticeVisible;

	if (bIsNoticeVisible)
	{
		SetWidgetSwitcher(2);
		SetVisibleSwitcher(true);
		if (bIsMenuVisible)
		{
			OnClickedlMenuBtn();
		}
		if(bIsChatVisible)
		{
			CloseButtonPressed();
		}
	}
	else
	{
		SetVisibleSwitcher(false);
	}
}

void UHM_MainBarWidget::OnClickedlightModeBtn()
{
	//나중
	//UI 라이트 모드로 변환 (기본모드)
}

void UHM_MainBarWidget::OnClickedDarkModeBtn()
{
	//나중
	//UI 다크모드로 변환
}

void UHM_MainBarWidget::OnClickedlMenuBtn()
{
	//이모티콘이랑 알림 버튼 on
	bIsMenuVisible = !bIsMenuVisible;
	if (bIsMenuVisible)
	{
		Can_0_Menu->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Can_0_Menu->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UHM_MainBarWidget::OnClickedChatBtn()
{
	//Main UI에 있는 채팅창 켜지게. 꺼지게. 델리게이트 연결함
	
}

void UHM_MainBarWidget::OnClickedSettingBtn()
{
	bIsSettingsVisible = !bIsSettingsVisible;
	//세팅창 켜지게
	if (bIsSettingsVisible)
	{
		if (bIsMenuVisible)
		{
			OnClickedlMenuBtn();
		}
		if(bIsChatVisible)
		{
			CloseButtonPressed();
		}
		SetWidgetSwitcher(3);
		SetVisibleSwitcher(true);
	}
	else if (!bIsSettingsVisible)
	{
		SetVisibleSwitcher(false);
	}
}

void UHM_MainBarWidget::OnClickedSettingBackBtn()
{
	SetVisibleSwitcher(false);
}
