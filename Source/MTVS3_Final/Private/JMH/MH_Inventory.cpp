// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_Inventory.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "JMH/MH_ItemBox_Sticker.h"
#include "JMH/MH_ItemBox_Ticket.h"
#include "JMH/MH_ItemBox_Title.h"
#include "LHM/HM_HttpActor3.h"
#include "Kismet/GameplayStatics.h"


void UMH_Inventory::NativeConstruct()
{
	Super::NativeConstruct();
	//칭호에서 시작
	OnClicked_PlayerTitle();
	//InitializeTabs();
	Btn_00_PlayerTitle->OnClicked.AddDynamic(this , &UMH_Inventory::OnClicked_PlayerTitle);
	Btn_01_Ticket->OnClicked.AddDynamic(this , &UMH_Inventory::OnClicked_Ticket);
	Btn_02_Sticker->OnClicked.AddDynamic(this , &UMH_Inventory::OnClicked_Sticker);
	Btn_Back_Inven->OnClicked.AddDynamic(this , &UMH_Inventory::CloseBtn_Inven);

	//타이틀 장착?
	Btn_Title_yes->OnClicked.AddDynamic(this , &UMH_Inventory::OnClickedTilteYesBtn);
	Btn_Title_no->OnClicked.AddDynamic(this , &UMH_Inventory::OnClickedTilteNoBtn);
	//타이틀 해제?
	Btn_Title_yes2->OnClicked.AddDynamic(this , &UMH_Inventory::OnClickedTilteYes2Btn);
	Btn_Title_no2->OnClicked.AddDynamic(this , &UMH_Inventory::OnClickedTilteNo2Btn);

	//test
	Btn_Title_Test->OnClicked.AddDynamic(this , &UMH_Inventory::OnClicked_Title_Test);
	Btn_Ticket_Test->OnClicked.AddDynamic(this , &UMH_Inventory::OnClicked_Ticket_Test);
	Btn_Sticker_Test->OnClicked.AddDynamic(this , &UMH_Inventory::OnClicked_Sticker_Test);
}

void UMH_Inventory::SetWidgetSwitcher(int32 num)
{
	WS_InvenWidgetSwitcher->SetActiveWidgetIndex(num);
}

void UMH_Inventory::ShowTitleEquipWin()
{
	Can_TitleEquipWin->SetVisibility(ESlateVisibility::Visible);
}

void UMH_Inventory::HideTitleWin()
{
	Can_TitleEquipWin->SetVisibility(ESlateVisibility::Hidden);
}


void UMH_Inventory::ShowTitleUnequipWin()
{
	Can_TitleUnequipWin->SetVisibility(ESlateVisibility::Visible);
}

void UMH_Inventory::HideTitleUnequipWin()
{
	Can_TitleUnequipWin->SetVisibility(ESlateVisibility::Hidden);
}


void UMH_Inventory::InitializeTabs()
{
	/*
		//데이터들이 저장될 HTTPInvenActor에서 정보 TArray로 받아오기
		AHM_HttpActor3* HTTP_Inven = Cast<AHM_HttpActor3>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
		if (!HTTP_Inven)
		{
			return;
		}
		if (HTTP_Inven)
		{
			// 받은 데이터가 비어 있지 않은지 확인
			const TArray<FTitleItemData>& TitleItems = HTTP_Inven->GetTitleItems();
			const TArray<FTicketItemData>& TicketItems = HTTP_Inven->GetTicketItems();
			const TArray<FStickerItemData>& StickerItems = HTTP_Inven->GetStickerItems();
	
			if (TitleItems.Num() > 0)
			{
				InitializeTitleTabs(TitleItems);
			}
			if (TicketItems.Num() > 0)
			{
				InitializeTicketTabs(TicketItems);
			}
			if (StickerItems.Num() > 0)
			{
				InitializeStickerTabs(StickerItems);
			}
		}*/
}

/*
void UMH_Inventory::InitializeTitleTabs(const TArray<FTitleItemData>& TitleItem)
{
	//타이틀 호리젠탈에 아이템 박스 넣어주기.
	Hori_InvenBox_00_Title->ClearChildren();

	for (const FTitleItemData& ItemData : TitleItem)
	{
		UMH_ItemBox_Title* ItemBox_Title = CreateWidget<UMH_ItemBox_Title>(this , TitleItemBoxFac);
		if (ItemBox_Title)
		{
			//ItemBox_Title->
			//ItemBox_Title->SetTitleData(ItemData); // 타이틀 데이터를 설정
			Hori_InvenBox_00_Title->AddChild(ItemBox_Title);
		}
	}
}

void UMH_Inventory::InitializeTicketTabs(const TArray<FTicketItemData>& TicketItems)
{
	///티켓 호리젠탈에 아이템 박스 넣어주기.
	Hori_InvenBox_01_Ticket->ClearChildren();

	for (const FTicketItemData& ItemData : TicketItems)
	{
		UMH_ItemBox_Ticket* ItemBox_Ticket = CreateWidget<UMH_ItemBox_Ticket>(this , TicketItemBoxFac);
		if (ItemBox_Ticket)
		{
			//ItemBox_Ticket->SetTicketData(ItemData); // 타이틀 데이터를 설정
			Hori_InvenBox_01_Ticket->AddChild(ItemBox_Ticket);
		}
	}
}

void UMH_Inventory::InitializeStickerTabs(const TArray<FStickerItemData>& StickerItems)
{
	//스티커 호리젠탈에 아이템 박스 넣어주기.
	Hori_InvenBox_02_Sticker->ClearChildren();

	for (const FStickerItemData& ItemData : StickerItems)
	{
		UMH_ItemBox_Sticker* ItemBox_Sticker = CreateWidget<UMH_ItemBox_Sticker>(this , StickerItemBoxFac);
		if (ItemBox_Sticker)
		{
			//ItemBox_Sticker->SetStickerData(ItemData); // 타이틀 데이터를 설정
			Hori_InvenBox_02_Sticker->AddChild(ItemBox_Sticker);
		}
	}
}*/


void UMH_Inventory::OnClicked_PlayerTitle()
{
	//타이틀
	SetWidgetSwitcher(0);
}

void UMH_Inventory::OnClicked_Ticket()
{
	//티켓
	SetWidgetSwitcher(1);
}

void UMH_Inventory::OnClicked_Sticker()
{
	//스티커
	SetWidgetSwitcher(2);
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

void UMH_Inventory::OnClickedTilteYesBtn()
{
	//클릭한 타이틀 장착 -> 클릭한 타이틀이 뭔지 저장할 변수 만들기
	//SetPlayerTitle();

	if (SelectedTitle == CurrentTitle)
	{
		return;
	}

	if (SelectedTitle)
	{
		// 이전에 선택된 아이템에서 프레임 제거
		RemoveFrame();
		//타이틀을 제거하시겠습니까? 창 뜨기
		//플레이어한테 칭호 해제
	}

	SelectedTitle = CurrentTitle;

	if (SelectedTitle)
	{
		// 현재 선택된 아이템에 프레임 추가
		AddFrame(CurrentTitle);
		// 프레임 위치 업데이트
		SetFramePosition(CurrentTitle);

		//플레이어한테 칭호 해제
	}

	OnClickedTilteNoBtn();
}

void UMH_Inventory::OnClickedTilteNoBtn()
{
	//창 지우기
	HideTitleWin();
}

void UMH_Inventory::OnClickedTilteYes2Btn()
{
	//칭호 해제하기

	SelectedTitle = nullptr;
	CurrentTitle = nullptr;
	//프레임 삭제
	RemoveFrame();
	HideTitleUnequipWin();
}

void UMH_Inventory::OnClickedTilteNo2Btn()
{
	//아니오
	HideTitleUnequipWin();
}

//1.타이틀버튼 누름
void UMH_Inventory::OnClickedTitleBtn(UMH_ItemBox_Title* ClickedItem)
{
	//2.타이틀을 적용하시겠습니까? 창뜸
	if (CurrentTitle == ClickedItem)
	{
		//칭호를 해제하시겠습니까?
		ShowTitleUnequipWin();
		return;
	}
	CurrentTitle = ClickedItem;
	ShowTitleEquipWin();
}

void UMH_Inventory::AddFrame(UMH_ItemBox_Title* ClickedItem)
{
	// 프레임 위치를 업데이트하며 보이도록 설정
	SetFramePosition(ClickedItem);
}

void UMH_Inventory::RemoveFrame()
{
	// 프레임을 숨김
	if (Img_Frame)
	{
		Img_Frame->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMH_Inventory::SetFramePosition(UMH_ItemBox_Title* ClickedItem)
{
	if (Img_Frame && ClickedItem)
	{
		GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("SetFrame111!"));

		if (UOverlaySlot** FoundSlot = OverlaySlotMap.Find(ClickedItem))
		{
			UOverlaySlot* OverlaySlot = *FoundSlot;
			GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("SetFrame222!"));
			if (OverlaySlot)
			{
				UOverlay* ParentOverlay = Cast<UOverlay>(OverlaySlot->Parent);
				if (ParentOverlay)
				{
					// Img_Frame이 이미 추가된 상태라면 이동만 하고, 그렇지 않으면 추가
					if (!ParentOverlay->HasChild(Img_Frame))
					{
						ParentOverlay->AddChildToOverlay(Img_Frame);
					}

					// 중앙 정렬 설정
					UOverlaySlot* FrameSlot = Cast<UOverlaySlot>(Img_Frame->Slot);
					if (FrameSlot)
					{
						FrameSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
						FrameSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
					}
				}
			}
			// Img_Frame의 크기를 부모 오버레이 크기에 맞추기
			Img_Frame->SetRenderScale(FVector2D(1.0f , 1.0f));
			Img_Frame->SetRenderTransformPivot(FVector2D(0.5f , 0.5f));

			// 프레임을 표시
			Img_Frame->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}
}

void UMH_Inventory::OnClicked_Title_Test()
{
	//타이틀 호리젠탈에 아이템 박스 넣어주기.
	UMH_ItemBox_Title* ItemBox_Title = CreateWidget<UMH_ItemBox_Title>(this , TitleItemBoxFac);
	UOverlay* OverlayTitle = NewObject<UOverlay>(this);

	if (ItemBox_Title)
	{
		Counter_Title++;
		ItemBox_Title->Text_Title->SetText(FText::FromString(FString::Printf(TEXT("%02d") , Counter_Title)));
		ItemBox_Title->OnClickedTitleBtn.AddDynamic(this , &UMH_Inventory::OnClickedTitleBtn);
		if (OverlayTitle)
		{
			// Overlay에 ItemBox_Title 추가
			UOverlaySlot* ItemSlot = OverlayTitle->AddChildToOverlay(ItemBox_Title);
			if (ItemSlot)
			{
				ItemSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
				ItemSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);

				// 아이템 박스와 오버레이 슬롯을 매핑하여 저장
				OverlaySlotMap.Add(ItemBox_Title , ItemSlot);
			}

			OverlayTitle->SetRenderTransformPivot(FVector2D(0.5f , 0.5f));
			Hori_InvenBox_00_Title->AddChild(OverlayTitle);
		}
	}
}

void UMH_Inventory::OnClicked_Ticket_Test()
{
	///티켓 호리젠탈에 아이템 박스 넣어주기.

	UMH_ItemBox_Ticket* ItemBox_Ticket = CreateWidget<UMH_ItemBox_Ticket>(this , TicketItemBoxFac);
	if (ItemBox_Ticket)
	{
		Counter_Ticket++;
		ItemBox_Ticket->Text_Ticket->SetText(FText::FromString(FString::Printf(TEXT("%02d") , Counter_Ticket)));
		//ItemBox_Ticket->SetTicketData(ItemData); // 타이틀 데이터를 설정
		Hori_InvenBox_01_Ticket->AddChild(ItemBox_Ticket);
	}
}

void UMH_Inventory::OnClicked_Sticker_Test()
{
	//스티커 호리젠탈에 아이템 박스 넣어주기.

	UMH_ItemBox_Sticker* ItemBox_Sticker = CreateWidget<UMH_ItemBox_Sticker>(this , StickerItemBoxFac);
	if (ItemBox_Sticker)
	{
		Counter_Sticker++;
		ItemBox_Sticker->Text_Sticker->SetText(FText::FromString(FString::Printf(TEXT("%02d") , Counter_Sticker)));
		//ItemBox_Sticker->SetStickerData(ItemData); // 타이틀 데이터를 설정
		Hori_InvenBox_02_Sticker->AddChild(ItemBox_Sticker);
	}
}
