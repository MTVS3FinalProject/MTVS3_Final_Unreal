// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_TicketingWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_TicketingWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	//위젯스위처설정
	UFUNCTION(BlueprintCallable)
	void SetWidgetSwitcher(int32 num);
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UWidgetSwitcher* WS_RegisterSwitcher;
	UFUNCTION(BlueprintCallable)
	void SetVisibleSwitcher(bool bVisible, int index);
	UFUNCTION(BlueprintCallable)
	void HiddenCanvas();
	
	UPROPERTY()
	FTimerHandle TimerHandle;
	
	//예매 접수
	//공연정보
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_ConcertName;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_ConcertDateY;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_ConcertDateM;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_ConcertDateD;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_ConcertTime;

	UFUNCTION()
	void SetConcertInfo(FString ConcertName, int32 ConcertDateY, int32 ConcertDateM, int32 ConcertDateD, FString ConcertTime );
	
	//좌석정보
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_SeatFloor;
	
	//좌석번호 셋팅
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_SeatID;
	UFUNCTION()
	void SetTextSeatID(int32 SeatFloor,FString SeatID);

	///////좌석 상태 표시 UI///////////
	// 오버레이 배열
	UPROPERTY(meta = (BindWidget))
	TArray<class UOverlay*> SeatOverlays;
	
	// 접수 가능석 상태를 저장 (True: 활성화, False: 비활성화)
	UPROPERTY()
	TArray<bool> SeatStates;

	// 초기화 함수
	UFUNCTION()
	void InitializeSeatsUI();

	// 현재 선택석 설정 함수
	UFUNCTION()
	void SetCurrentSelectedSeatUI(FString ChairTag);

	// 접수 가능석 업데이트 함수
	UFUNCTION()
	void UpdateReservedSeatsUI(TArray<int32> ReservedSeats);

	//좌석 추첨시간
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_TickettingDateY;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_TickettingDateM;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_TickettingDateD;

	UFUNCTION()
	void SetTickettingDate(int32 TickettingDateY,int32  TickettingDateM,int32  TickettingDateD);

	//게임예정 시작시간 //애는 다른곳에도 띄워야해서 따로
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_GameStartTime;
	UFUNCTION()
	void SetTextGameStartTime(FString CompetitionStartTime);

	//잔여접수 티켓 받아오기
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_RemainingTickets;
	UFUNCTION()
	void SetTextRemainingTicket(int32 RemainingTicket);

	//접수 여부에 따라 달라지는 UI
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UVerticalBox* Ver_Completed;
	UFUNCTION()
	void SetCompletedVisible(bool bVisible);

	//티켓 금액 받아오기
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_TicketPrice;
	UFUNCTION()
	void SetTextTicketPrice(int32 TicketPrice);
	

	
	//닫기 
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Back1;
	UFUNCTION()
	void OnClickedBackButton();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Confirm_Ticketting;
	UFUNCTION()
	void OnClickedConfirmButton();


	
	//추첨 입장전 게임 카운트다운
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_GameCountDown;
	UFUNCTION()
	void SetTextGameCountDown(FString GameCountDown);

	//접수한 좌석
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_SeatInfo;

	
	
	//접수한 좌석 구매 가격
	//UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	//class UTextBlock* Text_TicketPrice2;
	
	//접수마감시간 받아오기
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_TicketingDeadline;
	UFUNCTION()
	void SetTextTicketingDeadline(FString TicketingDeadline);
	

	//경쟁률
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_CompetitionRate;
	UFUNCTION()
	void SetTextCompetitionRate(int32 CompetitionRate);

	// KHJ
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UImage* Img_Loading;
	
	UPROPERTY(meta=(BindWidgetAnim),Transient)
	UWidgetAnimation* LoadingAnim;

	UFUNCTION()
	void SetLoadingActive(bool bIsActive);
	// KHJ

	//게임입장
	//UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	//class UButton*  Btn_Back2;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_GotoGameRoom;	
	UFUNCTION()
	void OnClickedGotoGameRoomButton();
	
	//취소
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Cancel_Ticketting1;
	UFUNCTION()
	void OnClickedCancelButton();
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Cancel_Ticketting2;
	UFUNCTION()
	void OnClickedCancelButton2();

	//UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	//class UButton*  Btn_Back1_1;
	//UFUNCTION()
	//void OnClickedBack1_1();

	//설정
	//PlayerVisible
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_PlayerVisible;
	UFUNCTION()
	void OnClickedPlayerVisibleButton();
	UFUNCTION()
	void SetPlayerVisible(bool bVisible);

	UPROPERTY()
	bool bIsOtherPlayerVisible = true;
	
    //Sound
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Sound;
	UFUNCTION()
	void OnClickedSoundButton();
	UFUNCTION()
	void SetSound(bool bIsSoundOn);

	UPROPERTY()
	bool bIsSetSound = true;
	
	
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Can_RegisterAnim_Off;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Can_RegisterAnim_On;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Over_Settings_On;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Over_Settings_Off;

	UPROPERTY()
	bool bIsVisible;

	//추첨장 입장 알림
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UCanvasPanel* Can_TicketBattleEntry;

	UFUNCTION()
	void SetBattleEntryVisible(bool bVisible);

	// KHJ
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UCanvasPanel* Can_HostBattleEntry;
	UFUNCTION()
	void SetHostBattleEntryVisible(bool bVisible);
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_HostBattleStart;
	UFUNCTION()
	void OnClickedHostBattleStartButton();
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UEditableText* T_SeatId;


	//좌석 접수 ui 이미지
	
	// ==================================
	// HttpActor2 공연장 입장 통신 테스트용

	
/*
	// 내가 접수한 좌석 조회
	UPROPERTY(VisibleAnywhere , meta = (BindWidget))
	class UButton* Btn_CancelRegisteredSeat;
	UFUNCTION()
	void OnClickedCancelRegisteredSeat();
	// 내가 접수한 좌석 조회
	UPROPERTY(VisibleAnywhere , meta = (BindWidget))
	class UButton* Btn_MyRegisterSeat;
	UFUNCTION()
	void OnClickedMyRegisterSeat();
	
	// 추첨 시작 알림
	UPROPERTY(VisibleAnywhere , meta = (BindWidget))
	class UButton* Btn_NoticeGameStart;
	UFUNCTION()
	void OnClickedNoticeGameStart();
	
	// 추첨 결과 요청
	UPROPERTY(VisibleAnywhere , meta = (BindWidget))
	class UButton* Btn_GameResult;
	UFUNCTION()
	void OnClickedGameResult();*/

};
