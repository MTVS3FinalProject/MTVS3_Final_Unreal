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
	UFUNCTION()
	void SetWidgetSwitcher(int32 num);
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UWidgetSwitcher* WS_RegisterSwitcher;
	UFUNCTION()
	void SetVisibleSwitcher(bool bVisible);
	

	//예매 접수
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
	
	//티켓 금액 받아오기
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_TicketPrice;
	UFUNCTION()
	void SetTextTicketPrice(int32 TicketPrice);

	//접수가능 티켓 수 받아오기
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_RemainingTickets;
	UFUNCTION()
	void SetTextRemainingTicket(int32 RemainingTicket);

	//접수마감시간 받아오기
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_TicketingDeadline;
	UFUNCTION()
	void SetTextTicketingDeadline(FString TicketingDeadline);

	//게임시작시간
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_GameStartTime;
	UFUNCTION()
	void SetTextGameStartTime(FString CompetitionStartTime);

	//경쟁률
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_CompetitionRate;
	UFUNCTION()
	void SetTextCompetitionRate(int32 CompetitionRate);

	//좌석번호 셋팅
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_SeatID;
	UFUNCTION()
	void SetTextSeatID(FString SeatID);

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
	
	//UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	//class UTextBlock* Tex_test;

	//UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	//class UTextBlock* Tex_test2;

	
	
};
