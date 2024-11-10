// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_MainBarWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/WidgetSwitcher.h"
#include "HJ/TTGameInstance.h"
#include "JMH/MH_Inventory.h"
#include "JMH/MH_NoticeWidget.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor2.h"
#include "LHM/HM_HttpActor3.h"

void UHM_MainBarWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Emoji->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedEmojiBtn);
	Btn_CollectionBook->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedCollectionBookBtn);
	Btn_Notice->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedNoticeBtn);
	Btn_lightMode->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedlightModeBtn);
	Btn_DarkMode->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedDarkModeBtn);
	Btn_Menu->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedMenuBtn);
	Btn_Chat->OnClicked.AddDynamic(this , &UHM_MainBarWidget::CloseButtonPressed);
	Btn_Setting->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedSettingBtn);
	Btn_Back_Settings->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedSettingBackBtn);

	// 현민 Http Test용
	Btn_HttpTest01->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedHttpTest01);
	Btn_HttpTest02->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedHttpTest02);
	Btn_HttpTest03->OnClicked.AddDynamic(this , &UHM_MainBarWidget::OnClickedHttpTest03);

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

void UHM_MainBarWidget::OnClickedEmojiBtn()
{
	//이모티콘 버튼 누르면
	bIsEmojiVisible = !bIsEmojiVisible;

	if (bIsEmojiVisible)
	{
		//스위처 0번 이모지 띄우기
		SetWidgetSwitcher(0);
		SetVisibleSwitcher(true);
		//메뉴바 켜져있으면 끄기
		if (bIsMenuVisible)
		{
			OnClickedMenuBtn();
		}
		//채팅창 켜져있으면 끄기
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

void UHM_MainBarWidget::OnClickedCollectionBookBtn()
{
	bIsCollectionBookVisible = !bIsCollectionBookVisible;

	if (bIsCollectionBookVisible)
	{
		SetWidgetSwitcher(2);
		SetVisibleSwitcher(true);
		if (bIsMenuVisible)
		{
			OnClickedMenuBtn();
		}
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

void UHM_MainBarWidget::OnClickedNoticeBtn()
{
	bIsNoticeVisible = !bIsNoticeVisible;

	if (bIsNoticeVisible)
	{
		SetVisibleSwitcher(true);
		SetWidgetSwitcher(1);
		if (bIsMenuVisible)
		{
			OnClickedMenuBtn();
		}
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

void UHM_MainBarWidget::OnClickedMenuBtn()
{
	//이모티콘이랑 알림 버튼 on
	bIsMenuVisible = !bIsMenuVisible;
	//이모티콘, 알림 비지블 켜져있으면 같이 꺼주기
	if (bIsNoticeVisible)
	{
		bIsNoticeVisible = !bIsNoticeVisible;
	}
	if (bIsEmojiVisible)
	{
		bIsEmojiVisible = !bIsEmojiVisible;
	}
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
		SetWidgetSwitcher(3);
		SetVisibleSwitcher(true);
		if (bIsMenuVisible)
		{
			OnClickedMenuBtn();
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
	if (bIsMenuVisible)
	{
		OnClickedMenuBtn();
	}
	if (bIsEmojiVisible)
	{
		OnClickedEmojiBtn();
	}
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

#pragma region 현민 Http Test용
void UHM_MainBarWidget::OnClickedHttpTest01()
{
	AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (!GI && !HttpActor2) return;

	// 내가 접수한 좌석 조회 요청(인벤토리?)
	UE_LOG(LogTemp , Log , TEXT("내가 접수한 좌석 조회 요청"));
	HttpActor2->ReqGetMyRegisteredSeat(GI->GetAccessToken());
}

void UHM_MainBarWidget::OnClickedHttpTest02()
{
	AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (!GI && !HttpActor2) return;
	// 예매 티켓 정보 불러오기 요청
	UE_LOG(LogTemp , Log , TEXT("예매 티켓 정보 불러오기 요청"));
	HttpActor2->ReqGetReservationInfo(GI->GetAccessToken());
}

void UHM_MainBarWidget::OnClickedHttpTest03()
{
	AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (!GI && !HttpActor2) return;
	// 좌석 결제 요청
	UE_LOG(LogTemp , Log , TEXT("좌석 결제 요청"));
	HttpActor2->ReqPostPaymentSeat(HttpActor2->GetMyReceptionSeatId() , GI->GetAccessToken());
}

void UHM_MainBarWidget::OnClickedHttpTest04()
{
	AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (!GI && !HttpActor3) return;
	// 인벤토리 정보 요청
	UE_LOG(LogTemp , Log , TEXT("인벤토리 정보 요청"));
	HttpActor3->ReqGetInventoryData(GI->GetAccessToken());
}

void UHM_MainBarWidget::OnClickedHttpTest05()
{
	AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (!GI && !HttpActor3) return;
	// Puzzle 결과, Sticker 획득 요청
	UE_LOG(LogTemp , Log , TEXT("Puzzle 결과, Sticker 획득 요청"));
	HttpActor3->ReqPostPuzzleResultAndGetSticker(1, GI->GetAccessToken());
}

void UHM_MainBarWidget::OnClickedHttpTest06()
{
	AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (!GI && !HttpActor3) return;
	// 커스텀 티켓 저장 요청
	UE_LOG(LogTemp , Log , TEXT("커스텀 티켓 저장 요청"));
	//HttpActor3->ReqPostSaveCustomTicket(, GI->GetAccessToken());
}

void UHM_MainBarWidget::OnClickedHttpTest07()
{
	AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (!GI && !HttpActor3) return;
	// 배경 생성 요청
	UE_LOG(LogTemp , Log , TEXT("배경 생성 요청"));
	HttpActor3->ReqGetBackground(GI->GetAccessToken());
}

void UHM_MainBarWidget::OnClickedHttpTest08()
{
	AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (!GI && !HttpActor3) return;
	// My 커스텀 티켓 목록 조회 요청
	UE_LOG(LogTemp , Log , TEXT("My 커스텀 티켓 목록 조회 요청"));
	HttpActor3->ReqGetCustomTicketList(GI->GetAccessToken());
}

void UHM_MainBarWidget::OnClickedHttpTest09()
{
	AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (!GI && !HttpActor3) return;
	// 티켓 커스텀 제작 입장 요청
	UE_LOG(LogTemp , Log , TEXT("티켓 커스텀 제작 입장 요청"));
	HttpActor3->ReqPostEnterTicketCustomization(GI->GetAccessToken());
}
#pragma endregion
