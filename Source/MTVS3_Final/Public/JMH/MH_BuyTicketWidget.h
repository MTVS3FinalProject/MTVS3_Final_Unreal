// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MH_BuyCoinsWidget.h"
#include "Blueprint/UserWidget.h"
#include "MH_BuyTicketWidget.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickedBuyTickerBack);

UCLASS()
class MTVS3_FINAL_API UMH_BuyTicketWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnClickedBuyTickerBack OnClickedBuyTickerBack;

	// 닫기 버튼 클릭 이벤트 설정
	UFUNCTION()
	void CloseButtonPressed()
	{
		// 델리게이트 호출
		OnClickedBuyTickerBack.Broadcast();
	}

	virtual void NativeConstruct() override;
	
	//수정 후 위젯 스위처 1029 // 0: QR 인증 . 1:인증성공  . 2:인증실패 . 3:좌석확인 . 4:가격확인 . 5: 예매자정보입력 . 6: 배송지입력 . 7:결제수단선택, 8: 결제완료. 9: 코인충전
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UWidgetSwitcher* WS_BuyTicketSwitcher;

	//위젯추가
	UPROPERTY()
	TSubclassOf<class UMH_BuyCoinsWidget> BuyCoinWidgetFac;
	UPROPERTY()
	class UMH_BuyCoinsWidget* BuyCoinUI;

	//위젯스위처설정
	UFUNCTION()
	void SetWidgetSwitcher(int32 num);


	//서버_좌석
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_SeatID01;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_SeatID02;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_SeatID03;

	//서버_콘서트 정보_이름
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_ConcertName_001;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_ConcertName_002;
	UFUNCTION()
	void SetTextConcertName(FString ConcertName);
	
	//서버_콘서트 정보_날짜 //현민 날짜 어디서 셋해주는지 모르겟
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_ConcertDate01Y;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_ConcertDate01M;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_ConcertDate01D;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_ConcertDate02Y;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_ConcertDate02M;
		UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_ConcertDate02D;
	
	UFUNCTION()
	void SetConcertInfo_BuyTicket(FString ConcertName, int32 ConcertDateY, int32 ConcertDateM, int32 ConcertDateD, FString ConcertTime );
	//서버_콘서트 정보_시간
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_ConcertTime;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_ConcertTime02;
	//서버_콘서트 정보_층
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_SeatFloor;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_SeatFloor02;
	UFUNCTION()
	void SetTextSeatID(int32 SeatFloor,FString SeatID);

	//서버_티켓 가격
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_TicketPrice01;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_TicketPrice02;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_TicketPrice03;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_ServiceCharge;

	//서버_예매자 이름
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_UserName;

	//서버_티켓매수
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_TicketNum;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_TicketNum2;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_TicketNum3;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_TicketNum4;
	//서버_코인
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_TotalCoin;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_TotalCoin_1;
	
	//현재 코인
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_CurrentCoin;

	//나중에 기능 추가
	//예매 수수료
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_ServiceFee;
	//배송료
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_DeliveryFee;


	//0 : 티켓 예매 하시겠습니까? + QR
	UFUNCTION()
	void SetTextTicketPrice(int32 TicketPrice);

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Confirm01;
	UFUNCTION()
	void OnClickedConfirm01Button();

	//예매자 정보 입력 확인버튼
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Confirm01_1;
	UFUNCTION()
	void OnClickedConfirm01_1Button();

	//배송지 입력 확인버튼
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Confirm02_1;
	UFUNCTION()
	void OnClickedConfirm02_1Button();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_RecentAddress;
	UFUNCTION()
	void OnClickedRecentAddress();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_MySeatMap;

	//1 예매자 정보 및 배송지 입력
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Confirm02;
	UFUNCTION()
	void OnClickedConfirm02Button();
	
	//2 QR 신원인증확인
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_QR;

	UFUNCTION()
	void SetQRImg(UTexture2D* newTexture);

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Confirm_QRUi1;
	UFUNCTION()
	void OnClickedConfirm_QRUi1Button();


	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UEditableText* EText_Name;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UEditableText* EText_PhoneNum;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UEditableText* EText_Address1;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UEditableText* EText_Address2;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Confirm_QRSuccess;
	UFUNCTION()
	void OnClickedConfirm_QRUi2SuccessButton();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Confirm_QRFailed;
	UFUNCTION()
	void OnClickedConfirm_QRUiFailedButton();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Back_QRFailed;

	UFUNCTION()
	void OnClickedBack_QRFailedButton();

// 뒤로가기 버튼들
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Back01;
	UFUNCTION()
	void OnClickedBack01Button();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Back02;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Back03;
	UFUNCTION()
	void OnClickedBack03Button();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Back04;
	UFUNCTION()
	void OnClickedBack04Button();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Back05;
	UFUNCTION()
	void OnClickedBack05Button();
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_Back06;
	UFUNCTION()
	void OnClickedBack06Button();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_BuyLater_1;
	UFUNCTION()
	void OnClickedBuyLaterButton();

	
	
	
	//결제진행
	//UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	//class UTextBlock* Tex_SeatID;
	//UFUNCTION()
	void SetTextSeatID(FString SeatID);

	// KHJ
	void SetTextWinnerSeatID(const FString& SeatID);
	// KHJ

	//UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	//class UTextBlock* Text_NeedCoin;


	//UPROPERTY(VisibleAnywhere , meta = (BindWidget))
	//class UTextBlock* Text_TicketPrice2;
	//UPROPERTY(VisibleAnywhere , meta = (BindWidget))
	//class UTextBlock* Text_TicketPrice2_2;

	UFUNCTION()
	void SetTextNeedCoin(int32 NeedCoin);

	UFUNCTION()
	void SetTextTicketNum(int32 TicketNum);

	UFUNCTION()
	void SetTextTotalCoin(int32 TotalCoin);

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_AddCoin;
	UFUNCTION()
	void OnClickedAddCoinButton();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_BuyTicketCoin;
	UFUNCTION()
	void OnClickedBuyTicketCoinButton();

	//3 : 티켓정보확인 _좌석확인


	//4 : 티켓정보확인 _가격

	//결제완료
	//UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	//class UTextBlock* Text_SeatID2;
	//여기
	//UFUNCTION()
	//void SetTextSeatID2(FString SeatID2);

	//UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	//class UTextBlock* Text_TicketNum3;

	//UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	//class UTextBlock* Text_TicketNum4;

	//UPROPERTY(VisibleAnywhere , meta = (BindWidget))
	//class UTextBlock* Text_TicketPrice3;


	UFUNCTION()
	void SetTextUserName(FString UserName);

	//현민 : 폰번호 다시불러와 쓸일이 없음.. 여기선
	//UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	//class UTextBlock* Tex_UserPhoneNum;
	UFUNCTION()
	void SetTextUserPhoneNum(FString UserPhoneNum);
	
	//현민 : 주소 다시불러와 쓸일이 없음.. 여기선
	//UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	//class UTextBlock* Tex_Address;
	UFUNCTION()
	void SetTextUserAddress(FString Address);

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_SaveTicket;
	UFUNCTION()
	void OnClickedSaveTicketButton();

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UButton* Btn_BuyTickerBack;
};
