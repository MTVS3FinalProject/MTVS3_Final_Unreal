// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_BuyCoinsWidget.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "HJ/TTGameInstance.h"

void UMH_BuyCoinsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_Coin_1000->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::OnClickedCoin_1000Button);
	Btn_Coin_5000->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::OnClickedCoin_5000Button);
	Btn_Coin_10000->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::OnClickedCoin_10000Button);
	Btn_Coin_50000->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::OnClickedCoin_50000Button);
	Btn_Coin_100000->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::OnClickedCoin_100000Button);
	Btn_BuyCoin->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::OnClickedBuyCoinButton);
	//SetVisibleCanvas(false);
	
}

void UMH_BuyCoinsWidget::SetVisibleCanvas(bool bVisible)
{
	if (bVisible)
	{
		Can_Main->SetVisibility(ESlateVisibility::Visible);
	}
	
	else if(!bVisible)
	{
		Can_Main->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMH_BuyCoinsWidget::OnClickedCoin_1000Button()
{
	AddCoinAmount = 1000;
}

void UMH_BuyCoinsWidget::OnClickedCoin_5000Button()
{
	AddCoinAmount = 5000;
}

void UMH_BuyCoinsWidget::OnClickedCoin_10000Button()
{
	AddCoinAmount = 10000;
}

void UMH_BuyCoinsWidget::OnClickedCoin_50000Button()
{
	AddCoinAmount = 50000;
}

void UMH_BuyCoinsWidget::OnClickedCoin_100000Button()
{
	AddCoinAmount = 100000;
}
//충전하기
void UMH_BuyCoinsWidget::OnClickedBuyCoinButton()
{
	//
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		gi->AddCoin(AddCoinAmount); //코인 충전
	}
}

//void UMH_BuyCoinsWidget::OnClickedBackButton()
//{
	//SetVisibility(ESlateVisibility::Hidden);
//}
