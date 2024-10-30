// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_BuyCoinsWidget.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClickedBuyCoinBack);

UCLASS()
class MTVS3_FINAL_API UMH_BuyCoinsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	FOnClickedBuyCoinBack OnClickedBuyCoinBack;
	
	// 닫기 버튼 클릭 이벤트 설정
	UFUNCTION()
	void CloseButtonPressed()
	{
		// 델리게이트 호출
		OnClickedBuyCoinBack.Broadcast();
	}
	
	virtual void NativeConstruct() override;
	//충전 결정 변수
	UPROPERTY()
	int32 AddCoinAmount = 0;

	//현재 코인

	//현재코인 텍스트
	//추가 코인
	//충전 후 코인 = AddCoinAmount+추가 코인
	
	//총 결제 금액 AddCoinAmount
	

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UCanvasPanel* Can_Main;
	
	UFUNCTION()
	void SetVisibleCanvas(bool bVisible);
	
	//충전 금액 결정 버튼
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Coin_5000;
	UFUNCTION()
	void OnClickedCoin_5000();
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Coin_10000;
	UFUNCTION()
	void OnClickedCoin_10000();
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Coin_50000;
	UFUNCTION()
	void OnClickedCoin_50000();
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Coin_100000;
	UFUNCTION()
	void OnClickedCoin_100000();
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_CustomCoin;
	UPROPERTY()
	int32 CustomCoinAmount;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_CustomCoinAmount;

	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UEditableText* EText_CustomCoinAmount;
	UFUNCTION()
	void SetCustomCoinAmount();

	
	UFUNCTION()
	void OnClickedCoin_Custom();
	//충전하기
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_BuyCoin;
	UFUNCTION()
	void OnClickedBuyCoinButton();

	//코인충전 버튼
	UPROPERTY()
	TArray<UButton*> ButtonArray;
	UFUNCTION()
	void OnButtonClicked(UButton* ClickedButton);
	//뒤로가기
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Back06_2;
	UFUNCTION()
	void OnClickedBackButton();
};
