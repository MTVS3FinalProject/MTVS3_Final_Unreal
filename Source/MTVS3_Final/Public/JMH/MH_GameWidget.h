// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_GameWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_GameWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UWidgetSwitcher* WS_GameWidgetSwitcher;

	//위젯스위처설정/ 0:게임시작,1: 결과, 2:승리(결제연결),3:패배, 4:예매
	UFUNCTION()
	void SetWidgetSwitcher(int32 num);
	
	//Hide
	UFUNCTION()
	void HideWidget();
	
	//Show
	UFUNCTION()
	void ShoeWidget();

	
//InGame
	//현재 인원
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Tex_CurrentPlayer;
	UFUNCTION()
	void SetTextCurrentPlayer(int32 CurrentPlayer);

	//현재 인원
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Tex_CountDown;
	UFUNCTION()
	void SetTextCountDown(FString CountDownTime);
	
	//룰렛1
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Tex_roulette1;
	UFUNCTION()
	void SetTextroulette1(FString roulette1);

	//룰렛2
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Tex_roulette2;
	UFUNCTION()
	void SetTextroulette2(int32 roulette2);

	//룰렛3
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Tex_roulette3;
	UFUNCTION()
	void SetTextroulette3(int32 roulette3);

	//룰렛3
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Tex_SeatNum1;
	UFUNCTION()
	void SetTextSeatNum1(FString SeatNum1);
	
	//룰렛3
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Tex_SeatNum2;
	UFUNCTION()
	void SetTextSeatNum2(FString SeatNum2);

	//예매성공
	//티켓 결제
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_BuyTicket;
	UFUNCTION()
	void OnClickedBuyTicket();
	
	//나중에 구매
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_BuyLater;
	UFUNCTION()
	void OnClickedBuyLater();

	//예매실패
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Tex_RemainingTicket;
	UFUNCTION()
	void SetTextRemainingTicket(int32 RemainingTicket);

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Back2;
	UFUNCTION()
	void OnClickedBackButton();

	//티켓예매 뒤로가기 버튼 . 게임진행
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Back01;
	UFUNCTION()
	void OnClickedBack01Button();

	
	
};
