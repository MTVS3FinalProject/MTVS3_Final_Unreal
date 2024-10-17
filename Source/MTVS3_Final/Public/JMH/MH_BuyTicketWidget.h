// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

		
	//Start
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UWidgetSwitcher* WS_BuyTicketSwitcher;
	
	//위젯스위처설정
	UFUNCTION()
	void SetWidgetSwitcher(int32 num);

	//0 티켓 예매정보 확인
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Confirm01;
	UFUNCTION()
	void OnClickedConfirm01Button();

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Back01;
	UFUNCTION()
	void OnClickedBack01Button();

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UImage* Img_MySeatMap;
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UEditableText* ETex_PhoneNum;

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
	
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Confirm_QRUi1;
	UFUNCTION()
	void OnClickedConfirm_QRUi1Button();

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton* Btn_Back_QRUi1;
	UFUNCTION()
	void OnClickedBack_QRUi1Button();
	
	
};
