// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_UserCoinWidget.h"

#include "Components/Button.h"

void UMH_UserCoinWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_BuyCoins->OnClicked.AddDynamic(this , &UMH_UserCoinWidget::OnClickedBuyCoinsButton);
}

void UMH_UserCoinWidget::OnClickedBuyCoinsButton()
{
	//MainUI -> 스위치 , 코인 충전 UI 이동
	 
}
