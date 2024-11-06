// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_Inventory.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"

void UMH_Inventory::NativeConstruct()
{
	Super::NativeConstruct();
	//칭호에서 시작
	OnClicked_PlayerTitle();

	Btn_00_PlayerTitle->OnClicked.AddDynamic(this , &UMH_Inventory::OnClicked_PlayerTitle);
	Btn_01_Ticket->OnClicked.AddDynamic(this , &UMH_Inventory::OnClicked_Ticket);
	Btn_02_Sticker->OnClicked.AddDynamic(this , &UMH_Inventory::OnClicked_Sticker);
	Btn_Back_Inven->OnClicked.AddDynamic(this,&UMH_Inventory::CloseBtn_Inven);
}

void UMH_Inventory::SetWidgetSwitcher(int32 num)
{
	WS_InvenWidgetSwitcher->SetActiveWidgetIndex(num);
}

void UMH_Inventory::OnClickedBack_Inven()
{
	//뒤로가기, mainUI 인벤꺼지게 델리게이트.
}

void UMH_Inventory::OnClicked_PlayerTitle()
{
	//칭호
	SetWidgetSwitcher(0);
	//현민
	//여기서 칭호 정보 받아오기
	//매희 - 셋해주는 함수 만들기
}

void UMH_Inventory::OnClicked_Ticket()
{
	//티켓
	SetWidgetSwitcher(1);
	//현민
	//여기서 티켓 정보 받아오기
	//매희 - 셋해주는 함수 만들기
}

void UMH_Inventory::OnClicked_Sticker()
{
	//스티커
	SetWidgetSwitcher(2);
	//현민
	//여기서 스티커 정보 받아오기
	//매희 - 셋해주는 함수 만들기
}

