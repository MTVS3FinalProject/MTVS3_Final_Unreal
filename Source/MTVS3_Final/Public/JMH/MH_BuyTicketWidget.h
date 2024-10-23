// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MH_BuyCoinsWidget.h"
#include "Blueprint/UserWidget.h"
#include "MH_BuyTicketWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_BuyTicketWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	virtual void NativeConstruct() override;

	//위젯 스위처 // 0: 예매정보확인, 계속진행 . 1:QR 인증 . 2:인증성공 . 3:인증실패 . 4:배송지입력 . 5: 결제진행 . 6: 결제완료 . 7:코인충전
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UWidgetSwitcher* WS_BuyTicketSwitcher;
	
	//위젯추가
	UPROPERTY()
	TSubclassOf<class UMH_BuyCoinsWidget> BuyCoinWidgetFac;
	UPROPERTY()
	class UMH_BuyCoinsWidget* BuyCoinUI;
		
	//위젯스위처설정
	UFUNCTION()
	void SetWidgetSwitcher(int32 num);

	//0 티켓 예매정보 확인
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Confirm01;
	UFUNCTION()
	void OnClickedConfirm01Button();

	//UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	//class UButton* Btn_Back01;
	//UFUNCTION()
	//void OnClickedBack01Button();

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UImage* Img_MySeatMap;

	//1 예매자 정보 및 배송지 입력
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Confirm02;
	UFUNCTION()
	void OnClickedConfirm02Button();

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Back02;
	UFUNCTION()
	void OnClickedBack02Button();
	
    //2 QR 신원인증확인
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UImage* Img_QR;

	UFUNCTION()
	void SetQRImg(UTexture2D* newTexture);
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Confirm_QRUi1;
	UFUNCTION()
	void OnClickedConfirm_QRUi1Button();

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Back_QRUi1;
	UFUNCTION()
	void OnClickedBack_QRUi1Button();

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UEditableText* EText_Name;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UEditableText* EText_PhoneNum;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UEditableText* EText_Address;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Confirm_QRSuccess;
	UFUNCTION()
	void OnClickedConfirm_QRUi2SuccessButton();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Confirm_QRFailed;
	UFUNCTION()
	void OnClickedConfirm_QRUiFailedButton();

	//결제진행
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Tex_SeatID;
	UFUNCTION()
	void SetTextSeatID(FString SeatID);

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_TicketNum;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_TicketNum2 = Text_TicketNum;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_TotalCoin;

	UFUNCTION()
	void SetTextTicketNum(FString TicketNum);
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_TicketPrice;
	UFUNCTION()
	void SetTextTicketPrice(int32 TicketPrice);

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_AddCoin;
	UFUNCTION()
	void OnClickedAddCoinButton();

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Back03;
	UFUNCTION()
	void OnClickedBack03Button();
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_BuyTicketCoin;
	UFUNCTION()
	void OnClickedBuyTicketCoinButton();

	

	//결제완료
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_SeatID2;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_TicketNum3 = Text_TicketNum;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Text_TotalCoin_1 = Text_TotalCoin; 

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Tex_UserName;
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Tex_UserPhoneNum;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UTextBlock* Tex_Address; 
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_SaveTicket;
	UFUNCTION()
	void OnClickedSaveTicketButton();

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Back04;
	UFUNCTION()
	void OnClickedBtn_Back04Button();

	//코인 충전창에서 뒤로가기
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Back05;
	UFUNCTION()
	void OnClickedBtn_Back05Button();
};
