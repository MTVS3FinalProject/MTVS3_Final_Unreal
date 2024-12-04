// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_MainBarWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/WidgetSwitcher.h"
#include "HJ/TTGameInstance.h"
#include "JMH/MH_Inventory.h"
#include "JMH/MH_NoticeWidget.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor3.h"

void UHM_MainBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//Btn_Emoji->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedEmojiBtn);
	Btn_CollectionBook->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedCollectionBookBtn);
	Btn_Notice->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedNoticeBtn);
	//Btn_lightMode->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedlightModeBtn);
	//Btn_DarkMode->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedDarkModeBtn);
	//Btn_Menu->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedMenuBtn);
	Btn_Chat->OnClicked.AddDynamic(this , &UHM_MainBarWidget::CloseButtonPressed);
	Btn_Setting->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedSettingBtn);
	Btn_Back_Settings->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedSettingBackBtn);

	SetVisibleSwitcher(false);

	if (WBP_inventoryUI)
	{
		WBP_inventoryUI->OnClickedBack_InvenBtn.AddDynamic(this , &UHM_MainBarWidget::CloseAllCategory);
	}

	if(WBP_NoticeUI)
	{
		WBP_NoticeUI->OnClickedBack_NoticeBtn.AddDynamic(this , &UHM_MainBarWidget::OnClickedNoticeBtn);
	}
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

void UHM_MainBarWidget::SetVisibilityState()
{
	SetVisibleSwitcher(true);
	if (bIsMenuVisible)
	{
		//OnClickedMenuBtn();
	}
	if (bIsChatVisible)
	{
		CloseButtonPressed();
	}
}

// void UHM_MainBarWidget::OnClickedEmojiBtn()
// {
// 	//이모티콘 버튼 누르면
// 	bIsEmojiVisible = !bIsEmojiVisible;
//
// 	if (bIsEmojiVisible)
// 	{
// 		//스위처 0번 이모지 띄우기
// 		SetWidgetSwitcher(0);
// 		SetVisibleSwitcher(true);
// 		//메뉴바 켜져있으면 끄기
// 		if (bIsMenuVisible)
// 		{
// 			//OnClickedMenuBtn();
// 		}
// 		//채팅창 켜져있으면 끄기
// 		if (bIsChatVisible)
// 		{
// 			CloseButtonPressed();
// 		}
// 	}
// 	else
// 	{
// 		SetVisibleSwitcher(false);
// 	}
// }

void UHM_MainBarWidget::OnClickedCollectionBookBtn()
{
	bIsCollectionBookVisible = !bIsCollectionBookVisible;

	if (bIsCollectionBookVisible)
	{
		//인벤 열기
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
		AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
		if (HttpActor3)
		{
			// 인벤토리 정보 요청
			HttpActor3->ReqGetInventoryData(GI->GetAccessToken());
		}
		SetWidgetSwitcher(2);
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
		SetVisibleSwitcher(true);
		SetWidgetSwitcher(1);
		//if (bIsMenuVisible)
		//{
			//OnClickedMenuBtn();
			// 우편함 열기
			UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
			AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
						UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
			if (HttpActor3)
			{
				// 우편함 조회 요청
				HttpActor3->ReqGetMailbox(GI->GetAccessToken());
			}
		//}
		if (bIsChatVisible)
		{
			CloseButtonPressed();
		}
	}
	else
	{
		SetVisibleSwitcher(false);
	}
}
/*
void UHM_MainBarWidget::OnClickedlightModeBtn()
{
	//나중
	//UI 라이트 모드로 변환 (기본모드)
}

void UHM_MainBarWidget::OnClickedDarkModeBtn()
{
	//나중
	//UI 다크모드로 변환
}*/

// void UHM_MainBarWidget::OnClickedMenuBtn()
// {
// 	//이모티콘이랑 알림 버튼 on
// 	bIsMenuVisible = !bIsMenuVisible;
// 	//이모티콘, 알림 비지블 켜져있으면 같이 꺼주기
// 	if (bIsNoticeVisible)
// 	{
// 		bIsNoticeVisible = !bIsNoticeVisible;
// 	}
// 	//if (bIsEmojiVisible)
// 	//{
// 	//	bIsEmojiVisible = !bIsEmojiVisible;
// 	//}
// 	if (bIsMenuVisible)
// 	{
// 		Can_0_Menu->SetVisibility(ESlateVisibility::Visible);
// 	}
// 	else
// 	{
// 		Can_0_Menu->SetVisibility(ESlateVisibility::Hidden);
// 	}
// }

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
		SetWidgetSwitcher(3);
		SetVisibleSwitcher(true);
		if (bIsMenuVisible)
		{
			//OnClickedMenuBtn();
		}
		if (bIsChatVisible)
		{
			CloseButtonPressed();
		}
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

void UHM_MainBarWidget::CloseAllCategory()
{
	// if (bIsMenuVisible)
	// {
	// 	OnClickedMenuBtn();
	// }
	// if (bIsEmojiVisible)
	// {
	// 	OnClickedEmojiBtn();
	// }
	if (bIsCollectionBookVisible)
	{
		OnClickedCollectionBookBtn();
	}
	if (bIsNoticeVisible)
	{
		OnClickedNoticeBtn();
	}
	if (bIsSettingsVisible)
	{
		OnClickedSettingBtn();
	}
	if (bIsChatVisible)
	{
		CloseButtonPressed();
	}
}

