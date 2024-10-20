﻿// Fill out your copyright notice in the Description page of Project Settings.

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
	void SetVisibleSwitcher(bool bVisible);

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

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UImage* Img_SeatInfoMap;
	UFUNCTION()
	void SetSeatInfoMap(FString SeatID);

	
	//닫기 
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Back1;
	UFUNCTION()
	void OnClickedBackButton();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Confirm_Ticketting;
	UFUNCTION()
	void OnClickedConfirmButton();

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Cancel_Ticketting1;
	UFUNCTION()
	void OnClickedCancelButton();

	
	


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


	//게임입장
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Back2;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_GotoGameRoom;	
	UFUNCTION()
	void OnClickedGotoGameRoomButton();
	
	//취소
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Cancel_Ticketting2;

	//설정
	//PlayerVisible
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_PlayerVisible;
	UFUNCTION()
	void OnClickedPlayerVisibleButton();
	UFUNCTION()
	void SetPlayerVisible(bool bVisible);
    //Sound
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Sound;
	UFUNCTION()
	void OnClickedSoundButton();
	UFUNCTION()
	void SetSound(bool bIsSoundOn);

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Can_RegisterAnim_Off;

	UPROPERTY(meta = (BindWidgetAnim), Transient)
	UWidgetAnimation* Can_RegisterAnim_On;

	//현민 테스트
	//좌석조회요청
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  btn_test1;
	UFUNCTION()
	void OnClickedtest1();
	//좌석접수요청
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  btn_test2;
	UFUNCTION()
	void OnClickedtest2();
	//좌석취소요청
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  btn_test3;
	UFUNCTION()
	void OnClickedtest3();
	//게임결과요청
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  btn_test4;
	UFUNCTION()
	void OnClickedtest4();
	//회원 인증용 QR요청
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  btn_test5;
	UFUNCTION()
	void OnClickedtest5();
	//회원사진 업로드확인 요청
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  btn_test6;
	UFUNCTION()
	void OnClickedtest6();
	//예매자 정보입력요청
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  btn_test7;
	UFUNCTION()
	void OnClickedtest7();

	//좌석 결제요청
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  btn_test8;
	UFUNCTION()
	void OnClickedtest8();
	
	//공연장 입장 요청
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  btn_test9;
	UFUNCTION()
	void OnClickedtest9();
};
