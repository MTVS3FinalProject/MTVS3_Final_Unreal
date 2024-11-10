// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_Inventory.generated.h"
/**
 * 
 */


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickedBack_Inven);

UCLASS()
class MTVS3_FINAL_API UMH_Inventory : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnClickedBack_Inven OnClickedBack_InvenBtn;

	bool bIsInvenVisible = false;

	UFUNCTION()
	void CloseBtn_Inven()
	{
		//상태 바꿔주고
		bIsInvenVisible = !bIsInvenVisible;
		//끄기
		OnClickedBack_InvenBtn.Broadcast();
	}

	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UWidgetSwitcher* WS_InvenWidgetSwitcher;
	//위젯스위처설정 
	UFUNCTION()
	void SetWidgetSwitcher(int32 num); //0:칭호,1:티켓,2:스티커

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Back_Inven;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UHorizontalBox* Hori_InvenBox_00_Title;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UHorizontalBox* Hori_InvenBox_01_Ticket;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UHorizontalBox* Hori_InvenBox_02_Sticker;

	//타이틀을 장착하시겠습니까?
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UCanvasPanel* Can_TitleEquipWin;
	//타이틀을 해제하시겠습니까?
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UCanvasPanel* Can_TitleUnequipWin;
	UFUNCTION()
	void ShowTitleEquipWin();
	UFUNCTION()
	void HideTitleWin();
	UFUNCTION()
	void ShowTitleUnequipWin();
	UFUNCTION()
	void HideTitleUnequipWin();
	
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Title_yes;
	UFUNCTION()
	void OnClickedTilteYesBtn();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Title_no;
	UFUNCTION()
	void OnClickedTilteNoBtn();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Title_yes2;
	UFUNCTION()
	void OnClickedTilteYes2Btn();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Title_no2;
	UFUNCTION()
	void OnClickedTilteNo2Btn();

	//처음 인벤토리 접속 할 때 아이템 데이터 받아오기
	UFUNCTION()
	void InitializeTabs();

	UPROPERTY()
	class UMH_ItemBox_Title* CurrentTitle;

/*
	//텝 초기화 함수들
	UFUNCTION()
	void InitializeTitleTabs(const TArray<FTitleItemData>& TitleItem);
	UFUNCTION()
	void InitializeTicketTabs(const TArray<FTicketItemData>& TicketItems);
	UFUNCTION()
	void InitializeStickerTabs(const TArray<FStickerItemData>& StickerItems);
*/

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_00_PlayerTitle;
	UFUNCTION()
	void OnClicked_PlayerTitle();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_01_Ticket;
	UFUNCTION()
	void OnClicked_Ticket();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_02_Sticker;
	UFUNCTION()
	void OnClicked_Sticker();

	//칭호,티켓,스티커 정보 받아오기
	UFUNCTION()
	void SetPlayerTitleInfo();
	UFUNCTION()
	void SetPlayerTicketsInfo();
	UFUNCTION()
	void SetPlayerStickerInfo();

	//플레이어 칭호 적용
	UFUNCTION()
	void SetPlayerTitle(int32 TitleID);

	//정보창 띄우기
	UFUNCTION()
	void ShowInfoWin();         
	

	//아이템 박스들
	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	TSubclassOf<class UMH_ItemBox_Title> TitleItemBoxFac;
	
	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	TSubclassOf<class UMH_ItemBox_Ticket> TicketItemBoxFac;

	UPROPERTY(EditAnywhere , BlueprintReadWrite)
	TSubclassOf<class UMH_ItemBox_Sticker> StickerItemBoxFac;

	UPROPERTY()
	class UMH_ItemBox_Title* SelectedTitle;

	UPROPERTY()
	class UMH_ItemBox_Ticket* SelectedTicket;
	
	UPROPERTY()
	class UMH_ItemBox_Sticker* SelectedSticker;

	//타이틀 박스 더블클릭시
	//UFUNCTION()
	//void HandleItemDoubleClicked(UMH_ItemBox_Title* ClickedItem);

	UFUNCTION()
	void OnClickedTitleBtn(UMH_ItemBox_Title* ClickedItem);
	
	//타이틀에 프레임 적용
	UPROPERTY(meta = (BindWidget))
	class UImage* Img_Frame;

	UFUNCTION()
	void AddFrame(UMH_ItemBox_Title* ClickedItem);
	UFUNCTION()
	void RemoveFrame();
	UFUNCTION()
	void SetFramePosition(UMH_ItemBox_Title* ClickedItem);



	//test 버튼들 //아이템박스 생성해주기
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Title_Test;
	UFUNCTION()
	void OnClicked_Title_Test();
	
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Ticket_Test;
	UFUNCTION()
	void OnClicked_Ticket_Test();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Sticker_Test;
	UFUNCTION()
	void OnClicked_Sticker_Test();
	
	int32 Counter_Title = 1;
	int32 Counter_Ticket = 1;
	int32 Counter_Sticker = 1;

	UPROPERTY()
	TMap<class UMH_ItemBox_Title*, class UOverlaySlot*> OverlaySlotMap;
	
	
};
