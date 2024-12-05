// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_Inventory.h"

#include "HttpModule.h"
#include "ImageUtils.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Interfaces/IHttpResponse.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "HJ/TTGameInstance.h"
#include "JMH/MH_ItemBox_Sticker.h"
#include "JMH/MH_ItemBox_Ticket.h"
#include "JMH/MH_ItemBox_Title.h"
#include "JMH/MH_ItemInfoBox.h"
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
	//Btn_Title_Test->OnClicked.AddDynamic(this , &UMH_Inventory::OnClicked_Title_Test);
	//Btn_Ticket_Test->OnClicked.AddDynamic(this , &UMH_Inventory::OnClicked_Ticket_Test);
	//Btn_Sticker_Test->OnClicked.AddDynamic(this , &UMH_Inventory::OnClicked_Sticker_Test);
	Buttons.Add(Btn_01_Ticket);
	Buttons.Add(Btn_02_Sticker);
	Buttons.Add(Btn_00_PlayerTitle);
	
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
		const TArray<FTitles>& TitleItems = HTTP_Inven->GetTitleItems();
		const TArray<FTickets>& TicketItems = HTTP_Inven->GetTicketItems();
		const TArray<FStickers>& StickerItems = HTTP_Inven->GetStickerItems();

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
	}
}

//현민
void UMH_Inventory::InitializeTitleTabs(const TArray<FTitles>& TitleItem)
{
	//타이틀 호리젠탈에 아이템 박스 넣어주기.
	Hori_InvenBox_00_Title->ClearChildren();

	for (const FTitles& ItemData : TitleItem)
	{
		//타이틀 호리젠탈에 아이템 박스 넣어주기.
		UMH_ItemBox_Title* ItemBox_Title = CreateWidget<UMH_ItemBox_Title>(this , TitleItemBoxFac);
		UOverlay* OverlayTitle = NewObject<UOverlay>(this);

		if (ItemBox_Title)
		{
			ItemBox_Title->ChangeColorTitleName(ItemData.titleRarity);
			ItemBox_Title->Text_Title->SetText(FText::FromString(ItemData.titleName));
			ItemBox_Title->OnClickedTitleBtn.AddDynamic(this , &UMH_Inventory::OnClickedTitleBtn);
			// 아이템박스 위젯을 가져와 델리게이트에 바인딩
			ItemBox_Title->OnItemHovered_Title.AddDynamic(this , &UMH_Inventory::OnHoveredTitleBtn);
			ItemBox_Title->OnItemUnHovered_Title.AddDynamic(this , &UMH_Inventory::OnUnHoveredTitleBtn);
			ItemBox_Title->SetInfoString_Title(ItemData.titleScript);
			ItemBox_Title->SetTitleID(ItemData.titleId);
			ItemBox_Title->SetTitleRarity(ItemData.titleRarity);
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
}

void UMH_Inventory::InitializeTicketTabs(const TArray<FTickets>& TicketItems)
{
	//콘서트네임,좌석정보,콘서트 이미지
	//티켓 호리젠탈에 아이템 박스 넣어주기.
	Hori_InvenBox_01_Ticket->ClearChildren();

	for (const FTickets& ItemData : TicketItems)
	{
		UMH_ItemBox_Ticket* ItemBox_Ticket = CreateWidget<UMH_ItemBox_Ticket>(this , TicketItemBoxFac);
		if (ItemBox_Ticket)
		{
			// URL에서 이미지 다운로드
			TSharedRef<IHttpRequest> ImageRequest = FHttpModule::Get().CreateRequest();
			ImageRequest->SetURL(ItemData.ticketImage);
			ImageRequest->SetVerb(TEXT("GET"));
			ItemBox_Ticket->OnItemHovered_Ticket.AddDynamic(this,&UMH_Inventory::OnHoveredTicketBtn);
			ItemBox_Ticket->OnItemUnHovered_Ticket.AddDynamic(this,&UMH_Inventory::OnUnHoveredTicketBtn);

			// 다운로드 완료 시 콜백 설정
			ImageRequest->OnProcessRequestComplete().BindLambda(
				[ItemBox_Ticket](FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
				{
					if (bWasSuccessful && Response.IsValid())
					{
						// 이미지 데이터 가져오기
						TArray<uint8> ImageData = Response->GetContent();

						// 텍스처로 변환
						UTexture2D* StickerTexture = FImageUtils::ImportBufferAsTexture2D(ImageData);
						if (StickerTexture && ItemBox_Ticket->Img_Item_Ticket)
						{
							// 이미지 위젯에 텍스처 적용
							ItemBox_Ticket->Img_Item_Ticket->SetBrushFromTexture(StickerTexture);
							UE_LOG(LogTemp , Log , TEXT("Ticket image set successfully"));
						}
						else
						{
							UE_LOG(LogTemp , Warning ,
							       TEXT("Failed to create TicketTexture or Img_Item_Ticket is null"));
						}
					}
					else
					{
						UE_LOG(LogTemp , Warning , TEXT("Failed to download Ticket image from URL: %s") ,
						       *Request->GetURL());
					}
				});
			ImageRequest->ProcessRequest();

			ItemBox_Ticket->SetInfoString_Ticket(ItemData.concertName,ItemData.seatInfo);
			//ItemBox_Ticket->SetTicketData(ItemData); // 타이틀 데이터를 설정
			Hori_InvenBox_01_Ticket->AddChild(ItemBox_Ticket);
		}
	}
}

void UMH_Inventory::InitializeStickerTabs(const TArray<FStickers>& StickerItems)
{
	//스티커 호리젠탈에 아이템 박스 넣어주기.
	Hori_InvenBox_02_Sticker->ClearChildren();

	for (const FStickers& ItemData : StickerItems)
	{
		UMH_ItemBox_Sticker* ItemBox_Sticker = CreateWidget<UMH_ItemBox_Sticker>(this , StickerItemBoxFac);
		if (ItemBox_Sticker)
		{
			// URL에서 이미지 다운로드
			TSharedRef<IHttpRequest> ImageRequest = FHttpModule::Get().CreateRequest();
			ImageRequest->SetURL(ItemData.stickerImage);
			ImageRequest->SetVerb(TEXT("GET"));

			// 아이템박스 위젯을 가져와 델리게이트에 바인딩
			ItemBox_Sticker->OnItemHovered_Sticker.AddDynamic(this , &UMH_Inventory::OnHoveredStickerBtn);
			ItemBox_Sticker->OnItemUnHovered_Sticker.AddDynamic(this , &UMH_Inventory::OnUnHoveredStickerBtn);

			// 다운로드 완료 시 콜백 설정
			ImageRequest->OnProcessRequestComplete().BindLambda(
				[ItemBox_Sticker](FHttpRequestPtr Request , FHttpResponsePtr Response , bool bWasSuccessful)
				{
					if (bWasSuccessful && Response.IsValid())
					{
						// 이미지 데이터 가져오기
						TArray<uint8> ImageData = Response->GetContent();

						// 텍스처로 변환
						UTexture2D* StickerTexture = FImageUtils::ImportBufferAsTexture2D(ImageData);
						if (StickerTexture && ItemBox_Sticker->Img_Item_Sticker)
						{
							// 이미지 위젯에 텍스처 적용
							ItemBox_Sticker->Img_Item_Sticker->SetBrushFromTexture(StickerTexture);
							UE_LOG(LogTemp , Log , TEXT("Sticker image set successfully"));
						}
						else
						{
							UE_LOG(LogTemp , Warning ,
							       TEXT("Failed to create StickerTexture or Img_Item_Sticker is null"));
						}
					}
					else
					{
						UE_LOG(LogTemp , Warning , TEXT("Failed to download sticker image from URL: %s") ,
						       *Request->GetURL());
					}
				});
			ImageRequest->ProcessRequest();

			ItemBox_Sticker->Text_Sticker->SetText(FText::FromString(ItemData.stickerName));
			ItemBox_Sticker->SetStickerRarity(ItemData.stickerRarity);
			ItemBox_Sticker->SetInfoString_Sticker(ItemData.stickerScript);

			UE_LOG(LogTemp , Log , TEXT("Sticker image set successfully"));
			Hori_InvenBox_02_Sticker->AddChild(ItemBox_Sticker);
			UE_LOG(LogTemp , Log , TEXT("InitializeStickerTabs"));
		}
	}
}

void UMH_Inventory::OnClicked_PlayerTitle()
{
	//타이틀
	SetWidgetSwitcher(0);
	UpdateButtonAlphas(2); // 세 번째 버튼 클릭 처리
	//플레이어 정보 타이틀 셋 되어있으면 해당 타이틀 프레임 생성 해주기. 
}

void UMH_Inventory::OnClicked_Ticket()
{
	//티켓
	SetWidgetSwitcher(1);
	UpdateButtonAlphas(0); // 첫 번째 버튼 클릭 처리
}

void UMH_Inventory::OnClicked_Sticker()
{
	//스티커
	SetWidgetSwitcher(2);
	UpdateButtonAlphas(1); // 두 번째 버튼 클릭 처리
}

void UMH_Inventory::UpdateButtonAlphas(int32 ClickedButtonIndex)
{
	for (int32 i = 0; i < Buttons.Num(); i++)
	{
		if (UButton* Button = Buttons[i])
		{
			/*
			// 버튼에서 자식 이미지를 찾습니다
			UImage* ButtonImage = Cast<UImage>(Button->GetChildAt(0));
			if (ButtonImage)
			{
				// 알파 값을 업데이트합니다
				float AlphaValue = (i == ClickedButtonIndex) ? 1.0f : 0.0f;
				FLinearColor NewColor = FLinearColor(1.0f, 1.0f, 1.0f, AlphaValue);
				ButtonImage->SetColorAndOpacity(NewColor);
			}*/
			
			// 클릭된 버튼이면 알파값 1, 그렇지 않으면 0
			float AlphaValue = (i == ClickedButtonIndex) ? 1.0f : 0.0f;

			// 버튼의 노멀 이미지 스타일 변경
			FButtonStyle ButtonStyle = Button->GetStyle();
			FSlateBrush NormalBrush = ButtonStyle.Normal;

			// 노멀 이미지 알파값 업데이트
			NormalBrush.TintColor = FSlateColor(FLinearColor(1.0f, 1.0f, 1.0f, AlphaValue));
			ButtonStyle.SetNormal(NormalBrush);

			// 스타일 업데이트
			Button->SetStyle(ButtonStyle);
		}
	}
}

void UMH_Inventory::SetPlayerTitle(int32 TitleID)
{
	//서버에 보내고.
	//서버에서 완료가 되면 플레이어 칭호 바꿔주기
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

		// 타이틀 해제 요청 통신
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
		AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
		if (HttpActor3 && GI)
		{
			HttpActor3->ReqGetNotEquipTheTitle(GI->GetAccessToken());
		}
	}

	SelectedTitle = CurrentTitle;

	if (SelectedTitle)
	{
		//플레이어한테 칭호 추가,
		// 현재 선택된 아이템에 프레임 추가
		AddFrame(CurrentTitle);
		// 프레임 위치 업데이트
		SetFramePosition(CurrentTitle);
		//등록할 때 필요
		UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
		AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
		if (HttpActor3 && GI)
		{
			HttpActor3->ReqGetEquipTheTitle(CurrentTitle->GetTitleID() , GI->GetAccessToken());
		}
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
	//해제할 때 필요
	// 타이틀 해제 요청 통신
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
	if (HttpActor3 && GI)
	{
		HttpActor3->ReqGetNotEquipTheTitle(GI->GetAccessToken());
	}
}

void UMH_Inventory::OnClickedTilteNo2Btn()
{
	//아니오
	HideTitleUnequipWin();
}

void UMH_Inventory::DestroyInfo(UMH_ItemInfoBox* DestroyBox)
{
	if (DestroyBox)
	{
		// UI에서 제거
		DestroyBox->RemoveFromParent();

		// 이후 가비지 컬렉터가 파괴하도록 둠
		DestroyBox = nullptr;
	}
}

void UMH_Inventory::OnHoveredTitleBtn(UMH_ItemBox_Title* HoveredItem)
{
	//인포 창 뜸.
	if (HoveredItem)
	{
		if (InfoBoxMap_Title.Contains(HoveredItem))
		{
			GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("InfoBoxMap!"));
			return;
		}
		
		UMH_ItemInfoBox* WBP_HoveredInfoTitlebox = CreateWidget<UMH_ItemInfoBox>(this , InfoBoxFac);
		if (WBP_HoveredInfoTitlebox)
		{
			GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red ,
			                                 TEXT("OnHoveredTitleBtn WBP_HoveredInfoTitlebox!"));
			Can_00->AddChildToCanvas(WBP_HoveredInfoTitlebox);
			
			// HoveredItem의 절대 위치를 가져오기
			FGeometry CachedGeometry = HoveredItem->GetCachedGeometry();
			FVector2D AbsolutePosition = CachedGeometry.GetAbsolutePosition();

			// 부모 패널의 좌표계로 변환
			FGeometry ParentGeometry = GetCachedGeometry(); // UMH_Inventory의 부모 패널
			FVector2D LocalPosition = ParentGeometry.AbsoluteToLocal(AbsolutePosition);

			// InfoBox를 HoveredItem 아래로 배치 (예: 50픽셀 아래)
			FVector2D Offset(-250.f , -120.0f);
			FVector2D InfoBoxPosition = LocalPosition + Offset;

			WBP_HoveredInfoTitlebox->SetRenderTranslation(InfoBoxPosition);
			WBP_HoveredInfoTitlebox->SetTextItemInfo(HoveredItem->GetInfoString_Title());
			WBP_HoveredInfoTitlebox->SetTextItemRarity(HoveredItem->GetTitleRarity());
			WBP_HoveredInfoTitlebox->PlayInfoTextAnim(true);
			InfoBoxMap_Title.Add(HoveredItem , WBP_HoveredInfoTitlebox);
		}
	}
}

void UMH_Inventory::OnUnHoveredTitleBtn(UMH_ItemBox_Title* UnHoveredItem)
{
	//인포 창 사라짐.
	if (UnHoveredItem && InfoBoxMap_Title.Contains(UnHoveredItem))
	{
		// 매핑에서 InfoBox 가져오기
		UMH_ItemInfoBox* WBP_HoveredInfoTitlebox = InfoBoxMap_Title[UnHoveredItem];
		if (WBP_HoveredInfoTitlebox)
		{
			GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red ,
			                                 TEXT("WBP_HoveredInfoTitlebox OnUnHoveredTitleBtn!"));
			WBP_HoveredInfoTitlebox->PlayInfoTextAnim(false);
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(
				TimerHandle ,
				[this, WBP_HoveredInfoTitlebox, UnHoveredItem]()
				{
					if (IsValid(this) && IsValid(WBP_HoveredInfoTitlebox))
					{
						DestroyInfo(WBP_HoveredInfoTitlebox);
						InfoBoxMap_Title.Remove(UnHoveredItem);
					}
				} ,
				1.0f , false);
		}
	}
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

void UMH_Inventory::OnHoveredStickerBtn(UMH_ItemBox_Sticker* HoveredItem_Sticker)
{
	//인포 창 뜸.
	if (HoveredItem_Sticker)
	{
		HoveredItem_Sticker->Text_ItemInfo_Sticker->SetText(
			FText::FromString(HoveredItem_Sticker->GetInfoString_Sticker()));
		HoveredItem_Sticker->Text_StickerRarity->SetText(FText::FromString(HoveredItem_Sticker->GetStickerRarity()));
		HoveredItem_Sticker->ShowInfo_Sticker();
	}
}

void UMH_Inventory::OnUnHoveredStickerBtn(UMH_ItemBox_Sticker* UnHoveredItem_Sticker)
{
	//인포 창 지움
	if (UnHoveredItem_Sticker)
	{
		UnHoveredItem_Sticker->HideInfo_Sticker();
	}
}

void UMH_Inventory::OnHoveredTicketBtn(UMH_ItemBox_Ticket* HoveredItem_Ticket)
{
	if(HoveredItem_Ticket)
	{
		HoveredItem_Ticket->Text_TicketName->SetText(FText::FromString(HoveredItem_Ticket->GetInfoNameString_Ticket()));
		HoveredItem_Ticket->Text_SeatInfo->SetText(FText::FromString(HoveredItem_Ticket->GetInfoString_Ticket()));
		HoveredItem_Ticket->ShowInfo_Ticket();
	}
}

void UMH_Inventory::OnUnHoveredTicketBtn(UMH_ItemBox_Ticket* UnHoveredItem_Ticket)
{
	if(UnHoveredItem_Ticket)
	{
		UnHoveredItem_Ticket->HideInfo_Ticket();
	}
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
		if (UOverlaySlot** FoundSlot = OverlaySlotMap.Find(ClickedItem))
		{
			UOverlaySlot* OverlaySlot = *FoundSlot;
			//GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("SetFrame!"));
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

/*
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
}*/
