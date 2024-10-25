// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MH_BuyCoinsWidget.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UMH_BuyCoinsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	virtual void NativeConstruct() override;
	//충전 결정 변수
	UPROPERTY()
	int32 AddCoinAmount = 0;

	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UCanvasPanel* Can_Main;
	
	UFUNCTION()
	void SetVisibleCanvas(bool bVisible);
	
	//충전 금액 결정 버튼
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Coin_1000;
	UFUNCTION()
	void OnClickedCoin_1000Button();
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Coin_5000;
	UFUNCTION()
	void OnClickedCoin_5000Button();
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Coin_10000;
	UFUNCTION()
	void OnClickedCoin_10000Button();
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Coin_50000;
	UFUNCTION()
	void OnClickedCoin_50000Button();
	UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	class UButton*  Btn_Coin_100000;
	UFUNCTION()
	void OnClickedCoin_100000Button();
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
	//UPROPERTY(VisibleAnywhere,meta=(BindWidget))
	//class UButton*  Btn_Back;
	//UFUNCTION()
	//void OnClickedBackButton();
};
