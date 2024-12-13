// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_MainBarWidget.h"
#include "AudioDevice.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Slider.h"
#include "Components/WidgetSwitcher.h"
#include "HJ/HallSoundManager.h"
#include "HJ/TTGameInstance.h"
#include "JMH/MH_Inventory.h"
#include "JMH/MH_NoticeWidget.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor3.h"
//#include "NsGui/VisualTreeHelper.h"
//#include <NsGui/FrameworkElement.h>
//#include "NoesisRuntime.h"

void UHM_MainBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	//Btn_Emoji->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedEmojiBtn);
	//Btn_lightMode->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedlightModeBtn);
	//Btn_DarkMode->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedDarkModeBtn);
	//Btn_Menu->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedMenuBtn);
	Btn_Notice->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedNoticeBtn);
	Btn_Notice->OnHovered.AddDynamic(this , &UHM_MainBarWidget::OnHoveredNoticeBtn);
	Btn_Notice->OnUnhovered.AddDynamic(this , &UHM_MainBarWidget::OnUnHoveredNoticeBtn);
	Btn_CollectionBook->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedCollectionBookBtn);
	Btn_CollectionBook->OnHovered.AddDynamic(this , &UHM_MainBarWidget::OnHoveredCollectionBookBtn);
	Btn_CollectionBook->OnUnhovered.AddDynamic(this , &UHM_MainBarWidget::OnUnHoveredCollectionBookBtn);
	Btn_Chat->OnClicked.AddDynamic(this , &UHM_MainBarWidget::CloseButtonPressed);
	Btn_Chat->OnHovered.AddDynamic(this , &UHM_MainBarWidget::OnHoveredChatBtn);
	Btn_Chat->OnUnhovered.AddDynamic(this , &UHM_MainBarWidget::OnUnHoveredChatBtn);
	Btn_Setting->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedSettingBtn);
	Btn_Setting->OnHovered.AddDynamic(this , &UHM_MainBarWidget::OnHoveredSettingBtn);
	Btn_Setting->OnUnhovered.AddDynamic(this , &UHM_MainBarWidget::OnUnHoveredSettingBtn);
	Btn_Back_Settings->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedSettingBackBtn);

	//KHJ
	Btn_GoToTutorial->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedGoToTutorialBtn);
	Btn_Credit->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedCreditBtn);
	Btn_Back_Credit->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedCreditBackBtn);
	Slider_BG->OnValueChanged.AddDynamic(this , &UHM_MainBarWidget::OnVolumeChanged);

	SetVisibleSwitcher(false);

	if (WBP_inventoryUI)
	{
		WBP_inventoryUI->OnClickedBack_InvenBtn.AddDynamic(this , &UHM_MainBarWidget::CloseAllCategory);
	}

	if (WBP_NoticeUI)
	{
		WBP_NoticeUI->OnClickedBack_NoticeBtn.AddDynamic(this , &UHM_MainBarWidget::OnClickedNoticeBtn);
	}

	MenuButtonGroups.Add({Btn_Notice , Img_NoticeBtn , Img_NoticeBG});
	MenuButtonGroups.Add({Btn_CollectionBook , Img_CollecBtn , Img_CollecBG});
	MenuButtonGroups.Add({Btn_Chat , Img_ChatBtn , Img_ChatBG});
	MenuButtonGroups.Add({Btn_Setting , Img_SettingBtn , Img_SettingBG});

	InitMenuBtn();
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
		bIsCollectionBookVisible = false;
		bIsNoticeVisible = false;
		bIsMenuVisible = false;
		bIsSettingsVisible = false;
	}
}

void UHM_MainBarWidget::SetWidgetSwitcher(int32 num)
{
	WS_Bar->SetActiveWidgetIndex(num);

	if (num == 3)
	{
		if (UTTGameInstance* GI = GetGameInstance<UTTGameInstance>())
		{
			CurrentVolume = GI->GetMasterVolume(); // CurrentVolume도 업데이트
			Slider_BG->SetValue(CurrentVolume);
		}
	}
}

void UHM_MainBarWidget::SetVisibilityState()
{
	//if (bIsChatVisible)
	//{
	//	CloseButtonPressed();
	//}
	SetVisibleSwitcher(true);
}

void UHM_MainBarWidget::OnclickedMenuBtn_chat()
{
	OnClickedMenuBtn(Btn_Chat);
}

void UHM_MainBarWidget::SetIsvisible_chat()
{
	bIsChatVisible = !bIsChatVisible;
}

void UHM_MainBarWidget::OnClickedCollectionBookBtn()
{
	bIsCollectionBookVisible = !bIsCollectionBookVisible;

	if (bIsCollectionBookVisible)
	{
		if (bIsChatVisible)
		{
			CloseButtonPressed();
		}
		OnClickedMenuBtn(Btn_CollectionBook);
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
		InitMenuBtn();
		SetVisibleSwitcher(false);
	}
}

void UHM_MainBarWidget::OnHoveredCollectionBookBtn()
{
	OnHoveredMenuBtn(true , Btn_CollectionBook);
}

void UHM_MainBarWidget::OnUnHoveredCollectionBookBtn()
{
	OnHoveredMenuBtn(false , Btn_CollectionBook);
}


void UHM_MainBarWidget::SetVisibleMenuBG(bool Visible , UImage* ImgBG)
{
	if (Visible)
	{
		ImgBG->SetVisibility(ESlateVisibility::Visible);
	}

	else
	{
		ImgBG->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UHM_MainBarWidget::OnHoveredMenuBtn(bool bIsHovere , UButton* OnHoveredButton)
{
	if (bIsHovere)
	{
		for (const FMenuButtonGroup& Group : MenuButtonGroups)
		{
			if (Group.Button == OnHoveredButton)
			{
				//텍스트 색상 변경

				Group.BtnImg->SetColorAndOpacity(FLinearColor::Black);
			}
		}
	}
	else
	{
		for (const FMenuButtonGroup& Group : MenuButtonGroups)
		{
			if (Group.Button == OnHoveredButton)
			{
				Group.BtnImg->SetColorAndOpacity(FLinearColor::White);
			}
		}
	}
}

void UHM_MainBarWidget::OnClickedMenuBtn(UButton* OnClickedButton)
{
	for (const FMenuButtonGroup& Group : MenuButtonGroups)
	{
		if (Group.Button == OnClickedButton)
		{
			//기본 배경 안보이게
			SetVisibleMenuBG(false , img_BaseBar);
			//클릭된 버튼 이미지 안보이게,
			SetVisibleMenuBG(false , Group.BtnImg);
			//클릭된 버튼 배경 보이게
			SetVisibleMenuBG(true , Group.BtnImg2);
		}
		else
		{
			// 나머지 버튼 이미지 보이게,
			SetVisibleMenuBG(true , Group.BtnImg);
			//나머지 버튼 배경 안보이게,
			SetVisibleMenuBG(false , Group.BtnImg2);
		}
	}
}

void UHM_MainBarWidget::InitMenuBtn()
{
	for (const FMenuButtonGroup& Group : MenuButtonGroups)
	{
		//버튼 이미지 모두 보이게,
		SetVisibleMenuBG(true , Group.BtnImg);
		//모든 버튼 배경 안보이게
		SetVisibleMenuBG(false , Group.BtnImg2);

		//기본 배경 보이게
		SetVisibleMenuBG(true , img_BaseBar);
	}
}

void UHM_MainBarWidget::OnClickedNoticeBtn()
{
	SetVisibleSwitcher(false);
	UE_LOG(LogTemp , Warning , TEXT("ClickedNoticeBtn"));
	bIsNoticeVisible = !bIsNoticeVisible;

	//우편함 열기
	if (bIsNoticeVisible)
	{
		UE_LOG(LogTemp , Warning , TEXT("ClickedNoticeBtn111"));
		//채팅창 열려있으면 닫기
		if (bIsChatVisible)
		{
			CloseButtonPressed();
		}
		//bar 우편함 보이게
		SetVisibleSwitcher(true);
		SetWidgetSwitcher(1);
		//채팅 버튼이미지 안보이게, 배경 보이게
		OnClickedMenuBtn(Btn_Notice);

		// 우편함 열기
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
		AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
		if (HttpActor3)
		{
			// 우편함 조회 요청
			HttpActor3->ReqGetMailbox(GI->GetAccessToken());
		}
	}
	//우편함 닫기
	else
	{
		UE_LOG(LogTemp , Warning , TEXT("ClickedNoticeBtn222"));
		//버튼 기본으로
		InitMenuBtn();
		//우편함 닫기
		SetVisibleSwitcher(false);
	}
}

void UHM_MainBarWidget::OnHoveredNoticeBtn()
{
	OnHoveredMenuBtn(true , Btn_Notice);
}

void UHM_MainBarWidget::OnUnHoveredNoticeBtn()
{
	OnHoveredMenuBtn(false , Btn_Notice);
}

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

void UHM_MainBarWidget::OnHoveredChatBtn()
{
	OnHoveredMenuBtn(true , Btn_Chat);
}

void UHM_MainBarWidget::OnUnHoveredChatBtn()
{
	OnHoveredMenuBtn(false , Btn_Chat);
}

void UHM_MainBarWidget::OnClickedSettingBtn()
{
	SetVisibleSwitcher(false);
	bIsSettingsVisible = !bIsSettingsVisible;
	UE_LOG(LogTemp , Warning , TEXT("ClickedSettingBtn"));
	//세팅창 켜지게
	if (bIsSettingsVisible)
	{
		UE_LOG(LogTemp , Warning , TEXT("ClickedSettingBtn111"));
		//채팅창 켜져있으면 끄기
		if (bIsChatVisible)
		{
			CloseButtonPressed();
		}

		//세팅창 열기
		SetVisibleSwitcher(true);
		SetWidgetSwitcher(3);

		//메뉴버튼이미지 안보이게, bar 배경 활성화
		OnClickedMenuBtn(Btn_Setting);
	}
	else if (!bIsSettingsVisible)
	{
		UE_LOG(LogTemp , Warning , TEXT("ClickedSettingBtn222"));
		//버튼 기본으로
		InitMenuBtn();
		//세팅창 닫기
		SetVisibleSwitcher(false);
	}
}

void UHM_MainBarWidget::OnHoveredSettingBtn()
{
	OnHoveredMenuBtn(true , Btn_Setting);
}

void UHM_MainBarWidget::OnUnHoveredSettingBtn()
{
	OnHoveredMenuBtn(false , Btn_Setting);
}

void UHM_MainBarWidget::OnClickedSettingBackBtn()
{
	SetVisibleSwitcher(false);
}

void UHM_MainBarWidget::CloseAllCategory()
{
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

void UHM_MainBarWidget::CloseAllCategory_Chat()
{
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
}

void UHM_MainBarWidget::OnVolumeChanged(float Value)
{
	CurrentVolume = Value;

	if (UTTGameInstance* GI = GetGameInstance<UTTGameInstance>())
	{
		GI->SetMasterVolume(Value);
	}

	// 모든 BGM AudioComponent 찾기
	AHallSoundManager* SoundManager = Cast<AHallSoundManager>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHallSoundManager::StaticClass()));

	if (SoundManager)
	{
		if (SoundManager->PlazaBGMAudioComponent)
		{
			SoundManager->PlazaBGMAudioComponent->SetVolumeMultiplier(Value);
		}
		if (SoundManager->ConcertBGMAudioComponent)
		{
			SoundManager->ConcertBGMAudioComponent->SetVolumeMultiplier(Value);
		}
	}
}

void UHM_MainBarWidget::OnClickedGoToTutorialBtn()
{
	SetVisibleSwitcher(false);
	OnClickedShowTutorialBtn.Broadcast();
}

void UHM_MainBarWidget::OnClickedCreditBtn()
{
	// 크레딧 페이지 보여주기
	SetWidgetSwitcher(4);
}

void UHM_MainBarWidget::OnClickedCreditBackBtn()
{
	// 환경설정으로 돌아가기
	SetWidgetSwitcher(3);
}
