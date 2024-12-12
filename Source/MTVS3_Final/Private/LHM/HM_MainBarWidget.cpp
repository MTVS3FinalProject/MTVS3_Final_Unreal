// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_MainBarWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/WidgetSwitcher.h"
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

	SetVisibleSwitcher(false);

	if (WBP_inventoryUI)
	{
		WBP_inventoryUI->OnClickedBack_InvenBtn.AddDynamic(this , &UHM_MainBarWidget::CloseAllCategory);
	}

	if(WBP_NoticeUI)
	{
		WBP_NoticeUI->OnClickedBack_NoticeBtn.AddDynamic(this , &UHM_MainBarWidget::OnClickedNoticeBtn);
	}
	/*
	// XAML 파일 경로 설정
	FString FullPath = FPaths::ProjectContentDir() + TEXT("UI/MyWidget.xaml");
	
	// XAML 파일 로드
	RootElement = Noesis::GUI::LoadXaml<Noesis::FrameworkElement>(TCHAR_TO_UTF8(*FullPath));
	if (!RootElement)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load XAML file: %s"), *FullPath);
		return;
	}
	
	// RiveControl 가져오기 (첫 번째 자식 요소로 접근)
	Noesis::Visual* FirstChild = Noesis::VisualTreeHelper::GetChild(RootElement.GetPtr(), 0);
	if (FirstChild)
	{
		RiveControl = Noesis::DynamicCast<NoesisApp::RiveControl*>(FirstChild);
		if (!RiveControl)
		{
			UE_LOG(LogTemp, Error, TEXT("RiveControl을 찾을 수 없습니다."));
		}
		else
		{
			UE_LOG(LogTemp, Log, TEXT("RiveControl 로드 성공."));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RootElement의 자식을 찾을 수 없습니다."));
	}*/
	
	MenuButtonGroups.Add({Btn_Notice,Img_NoticeBtn});
	MenuButtonGroups.Add({Btn_CollectionBook,Img_CollecBtn});
	MenuButtonGroups.Add({Btn_Chat,Img_ChatBtn});
	MenuButtonGroups.Add({Btn_Setting,Img_SettingBtn});

}

//Rive
	/*
void UHM_MainBarWidget::ActivateRiveTrigger(const FString& TriggerName)
{
	if (!RiveControl)
	{
		UE_LOG(LogTemp, Error, TEXT("RiveControl is not initialized."));
		return;
	}
}
	*/

	/*
void UHM_MainBarWidget::SetRiveParameter(const FString& ParameterName, float Value)
{
	if (RiveControl)
	{
		UE_LOG(LogTemp, Error, TEXT("RiveControl is not initialized."));
		return;
	}
	
	// 매개변수 값을 설정하기 위해 SetInputValue 호출
	RiveControl->SetInputValue(TCHAR_TO_UTF8(*ParameterName), Noesis::Boxing::Box(Value));
	UE_LOG(LogTemp, Log, TEXT("Parameter '%s' set to value %f."), *ParameterName, Value);
}
	*/

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

void UHM_MainBarWidget::OnHoveredCollectionBookBtn()
{
	OnHoveredMenuBtn(true,Btn_CollectionBook);
}

void UHM_MainBarWidget::OnUnHoveredCollectionBookBtn()
{
	OnHoveredMenuBtn(false,Btn_CollectionBook);
}

void UHM_MainBarWidget::OnHoveredMenuBtn(bool bIsHovere, UButton* OnHoveredButton)
{
	if(bIsHovere)
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

void UHM_MainBarWidget::OnClickedMenuBtn(bool bIsClick, UButton* OnClickedButton)
{
	
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

void UHM_MainBarWidget::OnHoveredNoticeBtn()
{
	OnHoveredMenuBtn(true,Btn_Notice);
}

void UHM_MainBarWidget::OnUnHoveredNoticeBtn()
{
	OnHoveredMenuBtn(false,Btn_Notice);
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

void UHM_MainBarWidget::OnHoveredChatBtn()
{
	OnHoveredMenuBtn(true,Btn_Chat);
}

void UHM_MainBarWidget::OnUnHoveredChatBtn()
{
	OnHoveredMenuBtn(false,Btn_Chat);
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

void UHM_MainBarWidget::OnHoveredSettingBtn()
{
	OnHoveredMenuBtn(true,Btn_Setting);
}

void UHM_MainBarWidget::OnUnHoveredSettingBtn()
{
	OnHoveredMenuBtn(false,Btn_Setting);
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

