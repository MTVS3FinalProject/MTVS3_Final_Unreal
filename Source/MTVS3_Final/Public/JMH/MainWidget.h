// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MH_BuyTicketWidget.h"
#include "MH_Interaction.h"
#include "MH_TTHUD.h"
#include "Blueprint/UserWidget.h"
#include "LHM/HM_MainBarWidget.h"
#include "MH_ErrorMessage.h"
#include "MainWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMainWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UWidgetSwitcher* WS_MainWidgetSwitcher;

	//위젯스위처설정 
	UFUNCTION()
	void SetWidgetSwitcher(int32 num); //0:미니맵,1:승리(결제연결),2:패배, 3:BuyTicket, 4:BuyCoin, 5:SelectConcert, 6:ExitWindow

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UCanvasPanel* Can_Main;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UCanvasPanel* Can_BuyTicket;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UCanvasPanel* Can_Interaction;
	
	//Error Message
	UPROPERTY(meta=(BindWidget))
	UMH_ErrorMessage*  WBP_MH_ErrorMessage02;

	//Login
	UFUNCTION()
	void ShowMainErrorMessage(FString ErrorMS);
	
	UFUNCTION()
	void HideMainErrorMessage();


	UFUNCTION()
	void SetVisibleCanvas(bool bVisible);

	//콘서트 입장
	UFUNCTION()
	void GoToConcertHall();

	//미니맵
	//현재시간 받아오기
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Tex_CurrentTime;
	UFUNCTION()
	void SetTextCurrentTime(FString CurrentTime);

	//로비로 나가기
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_BackMain;
	UFUNCTION()
	void OnClickedBackMain();

	//게임 나가기
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_ExitMainWin;
	UFUNCTION()
	void OnClickedExitMainWin();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_ExitMain;
	UFUNCTION()
	void OnClickedExit();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_BackMainWin;

	//==========추가

	//BuyTicketWidget 서버가 호출 가능하게
	UPROPERTY(meta = (BindWidget))
	UMH_BuyTicketWidget* BuyTicketWidget;

	// BuyWidget 컴포넌트에 대한 참조 반환 함수
	UFUNCTION(BlueprintCallable , Category="UI")
	UMH_BuyTicketWidget* GetBuyTicketWidget() const
	{
		return BuyTicketWidget;
	}

	//TicketCustomWidget 서버가 호출 가능하게
	UPROPERTY(meta = (BindWidget))
	UHM_TicketCustom* TicketCustomWidget;

	// TicketCustomWidget 컴포넌트에 대한 참조 반환 함수
	UFUNCTION(BlueprintCallable , Category="UI")
	UHM_TicketCustom* GetTicketCustomWidget() const
	{
		return TicketCustomWidget;
	}

	//FinalTicket 서버가 호출 가능하게
	UPROPERTY(meta = (BindWidget))
	UHM_FinalTicket* FinalTicketWidget;

	// TicketCustomWidget 컴포넌트에 대한 참조 반환 함수
	UFUNCTION(BlueprintCallable , Category="UI")
	UHM_FinalTicket* GetFinalTicketWidget() const
	{
		return FinalTicketWidget;
	}

	bool bIsChatVisible = false;

	UPROPERTY(meta=(BindWidget))
	class UMH_Chatting* WBP_Chatting;

	UPROPERTY(meta = (BindWidget))
	UMH_BuyCoinsWidget* BuyCoinsWidget;

	UPROPERTY(meta = (BindWidget))
	UHM_MainBarWidget* WBP_MH_MainBar;
	
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UCanvasPanel* Can_Chat_Emoji;
	
	UFUNCTION()
	void ShowChatUI();

	
	//0:Minimap
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_MinimapImg;

	UFUNCTION()
	void SetMinimapImage(UTexture2D* img);
	//1: Win
	//티켓 결제
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_BuyTicket;
	UFUNCTION()
	void OnClickedBuyTicket();
	UFUNCTION()
	void OnClickedBuyTicketBack();
	//좌석번호
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Tex_SeatNum1;
	UFUNCTION()
	void SetTextSeatNum1(FString SeatNum1);

	//나중에 구매
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_BuyLater;
	UFUNCTION()
	void OnClickedBack_Map();

	//2: Fail
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_FailBack;

	//잔여접수 티켓 받아오기
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_RemainingTickets;
	UFUNCTION()
	void SetTextRemainingTicket(int32 RemainingTicket);

	//4: BuyCoins
	//UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	//class UButton* Btn_BuyCoinsBack2;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_BuyCoins;
	UFUNCTION()
	void OnClickedBuyCoinsButton();

	//5: SelectConcert
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Concert01;
	UFUNCTION()
	void OnClickedConcert01();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Concert02;
	UFUNCTION()
	void OnClickedConcert02();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Concert03;
	UFUNCTION()
	void OnClickedConcert03();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Concert04;
	UFUNCTION()
	void OnClickedConcert04();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Concert05;
	UFUNCTION()
	void OnClickedConcert05();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_SelectConcertBack;

	UPROPERTY()
	TArray<UCanvasPanel*> ConcertInfoCanvas;

	//UFUNCTION()
	//void SetCan_ConcertInfoVisibility(UCanvasPanel* TargetCanvas);
	
	//입장하기 버튼
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Confirm_Concert;
	UFUNCTION()
	void OnClickedConfirm_Concert();

	UPROPERTY()
	int32 ConcertNum = 0;

	//왼쪽 버튼 클릭
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_ConcertL;
	UFUNCTION()
	void OnClickedConcertL();

	//오른쪽버튼 클릭
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_ConcertR;
	UFUNCTION()
	void OnClickedConcertR();

	//콘서트선택 위젯 애니메이션
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* TicketImgAnim01;

	UFUNCTION()
	void OnTicketWidgetClose();
	UFUNCTION()
	void OnClickedCustomTicketSaveButton();

	//UI 애니메이션
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* ConcertAnim1;
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* ConcertAnim2;
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* ConcertAnim3;
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* ConcertAnim4;
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* ConcertAnim5;

	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* ConcertAnim1_0;
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* ConcertAnim2_1;
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* ConcertAnim3_2;
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* ConcertAnim4_3;
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* ConcertAnim5_4;

	UFUNCTION()
	void SelectConcertAnim(bool bIsRightBtn , int32 AnimNum);

	UPROPERTY()
	AMH_TTHUD* TTHUD;

	//콘서트 정보 애니메이션
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* ConcertInfoAnim01;
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* ConcertInfoAnim02;
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* ConcertInfoAnim03;
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* ConcertInfoAnim04;
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* ConcertInfoAnim05;
	
	UFUNCTION()
	void SelectConcertInfoAnim(int32 InfoAnimNum);

	//이전 애니메이션이 있다면 없어지고 -> 새로운건 생기게
	UPROPERTY()
	UCanvasPanel* PreviousCanvas; // 이전 캔버스를 기억하는 변수
    //이전 캔버스가 있었는지 없었는지 기억하는 불변수. 초기화는 콘서트셀렉창 닫을 때.
	UPROPERTY()
	bool bHasPreviousCanvas = false; // 이전 캔버스가 있었는지 여부를 저장
	
	//Info canvas
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UCanvasPanel* Can_ConcertInfo01;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UCanvasPanel* Can_ConcertInfo02;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UCanvasPanel* Can_ConcertInfo03;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UCanvasPanel* Can_ConcertInfo04;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UCanvasPanel* Can_ConcertInfo05;

	UPROPERTY()
	class UCanvasPanel* infoCanvas;
	UPROPERTY()
	TArray<UCanvasPanel*> InfoCanvasPanels;
	
	UFUNCTION()
	void SetInfoCanvas(int32 InfoAnimNum);

	UFUNCTION()
	void SetInfoCanvasVisibility(UCanvasPanel* TargetCanvas,int32 InfoAnimNum);

	//티켓예매 알람
	//티켓팅 시간대
	//상호작용 버튼

	//희진이
	//WBP_InteractionUI 여기서 스위쳐함수 찾아서 바꿔주기
	UPROPERTY(meta = (BindWidget))
	UMH_Interaction* WBP_InteractionUI;
	//Interaction 캔버스 비지블
	UFUNCTION()
	void SetVisibleInteractionCan(bool visible);
	UFUNCTION()
	void HandlePaymentPostpone();

	// 10: 튜토리얼
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UWidgetSwitcher* WS_Tutorial;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_TutorialStart;
	UFUNCTION()
	void OnClickedTutorialStart();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_TutorialSkip;
	UFUNCTION()
	void OnClickedTutorialSkip();
	
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Right1;
	UFUNCTION()
	void OnClickedRight1();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Left2;
	UFUNCTION()
	void OnClickedLeft2();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Right2;
	UFUNCTION()
	void OnClickedRight2();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Left3;
	UFUNCTION()
	void OnClickedLeft3();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Right3;
	UFUNCTION()
	void OnClickedRight3();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Left4;
	UFUNCTION()
	void OnClickedLeft4();
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UButton* Btn_Right4;
	UFUNCTION()
	void OnClickedRight4();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Left5;
	UFUNCTION()
	void OnClickedLeft5();
	UPROPERTY(VisibleAnywhere, meta=(BindWidget))
	class UButton* Btn_Right5;
	UFUNCTION()
	void OnClickedRight5();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Left6;
	UFUNCTION()
	void OnClickedLeft6();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_TutorialEnd;
	UFUNCTION()
	void OnClickedTutorialEnd();

	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* FadeOutAnim;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_TitlePlaza;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_TitleConcertHall;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_TitleCommunityHall;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_TitleStyleLounge;

	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* TitlePlazaAnim;
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* TitleConcertHallAnim;
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* TitleCommunityHallAnim;
	UPROPERTY(meta = (BindWidgetAnim) , Transient)
	UWidgetAnimation* TitleStyleLoungeAnim;

	void HideAllTitle();

	UFUNCTION(BlueprintCallable)
	void PlayTitleAnim(int32 TitleNum);
};
