// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MH_BuyTicketWidget.h"
#include "Blueprint/UserWidget.h"
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

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UWidgetSwitcher* WS_MainWidgetSwitcher;

	//위젯스위처설정 
	UFUNCTION()
	void SetWidgetSwitcher(int32 num);//0:미니맵,1:승리(결제연결),2:패배, 3:BuyTicket, 4:BuyCoin, 5:SelectConcert
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UCanvasPanel* Can_Main;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UCanvasPanel* Can_BuyTicket;


	
	UFUNCTION()
	void SetVisibleCanvas(bool bVisible);
	
	//미니맵
	//현재시간 받아오기
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Tex_CurrentTime;
	UFUNCTION()
	void SetTextCurrentTime(FString CurrentTime);

	//로비로 나가기
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_BackMain;
	UFUNCTION()
	void OnClickedBackMain();
	
	//게임 나가기
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_ExitMain;
	UFUNCTION()
	void OnClickedExitMain();

//==========추가

	//BuyTicketWidget 서버가 호출 가능하게
	UPROPERTY(meta = (BindWidget))
	UMH_BuyTicketWidget* BuyTicketWidget;
	
	// BuyWidget 컴포넌트에 대한 참조 반환 함수
	UFUNCTION(BlueprintCallable, Category="UI")
	UMH_BuyTicketWidget* GetBuyTicketWidget() const
	{
		return BuyTicketWidget;
	}
	
	//0:Minimap
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UImage* Img_MinimapImg;

	UFUNCTION()
	void SetMinimapImage(UTexture2D* img);
	//1: Win
	//티켓 결제
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_BuyTicket;
	UFUNCTION()
	void OnClickedBuyTicket();
	//좌석번호
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Tex_SeatNum1;
	UFUNCTION()
	void SetTextSeatNum1(FString SeatNum1);
	//좌석번호
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Tex_SeatNum2;
	UFUNCTION()
	void SetTextSeatNum2(FString SeatNum2);
	
	//나중에 구매
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_BuyLater;
	UFUNCTION()
	void OnClickedBack_Map();

	//2: Fail
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_FailBack;
	
	//잔여접수 티켓 받아오기
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_RemainingTickets;
	UFUNCTION()
	void SetTextRemainingTicket(int32 RemainingTicket);

	//3: BuyTicket
	//UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	//class UButton* Btn_BuyTickerBack;
	//4: BuyCoins
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_BuyCoinsBack2;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_BuyCoins;
	UFUNCTION()
	void OnClickedBuyCoinsButton();
	
	//5: SelectConcert
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Concert01;
	UFUNCTION()
	void OnClickedConcert01();
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_SelectConcertBack;

	
	
	//티켓예매 알람
	//티켓팅 시간대
	//상호작용 버튼

	
};
