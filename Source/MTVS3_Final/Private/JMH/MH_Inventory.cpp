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

void UMH_Inventory::SetPlayerTitleInfo()
{
	//WBP_ItemBox에 타이틀 텍스트 넣어서(매개변수 1. Text or Image) 저장
	//생성
	//Hori_InvenBox_00_PlayerTitle 자식으로 셋해주기
	//여러개 일 수 있음.(배열로 받아오기?) for문 돌려서 셋해주기?

	//On Hovered되면 플레이어 타이틀에 대한 정보 표시(매개변수 2. String) 저장

	
	//>여기는 칭호 box 부분
	//더블클릭하면 칭호 적용시켜주기. 또 더블클릭하면 해제 해주기.
	//인벤토리 위젯에서는 프레임 씌워주기, 지워주기.
}

void UMH_Inventory::SetPlayerTicketsInfo()
{
	//WBP_ItemBox에 티켓이미지 넣어서(매개변수 1. Image)
	//생성
	//Hori_InvenBox_01_Ticket 자식으로 셋해주기
	//여러개 일 수 있음.(배열로 받아오기?) for문 돌려서 셋해주기?

	//On Hovered되면 티켓에 대한 정보 표시(매개변수 2. String) 
}

void UMH_Inventory::SetPlayerStickerInfo()
{
	//WBP_ItemBox에 스티커 이미지 넣어서 (매개변수 1. Image)
	//생성
	//Hori_InvenBox_02_Sticker 자식으로 셋해주기
	//여러개 일 수 있음.(배열로 받아오기?) for문 돌려서 셋해주기?
	
	//On Hovered되면 스티커에 대한 정보 표시(매개변수 2. String) 
}

void UMH_Inventory::SetPlayerTitle(int32 TitleID)
{
	//플레이어에 있는 칭호 위젯에 칭호 적용.
	//플레이어에 칭호 적용 되었다는  bIsEquipped = true; 변경
	
	
}

void UMH_Inventory::ShowInfoWin()
{
	//애니메이션
	//창 켜지고
	//String 해당 정보 가져오기.->SetText
	//정보 구별은..? 내가 따로 저장해두고 가져와야 할듯
	//호버된 위치에서 떠야하는데? 정해진 버튼 위치에서 오른쪽 아래로 떠야겠다

}

