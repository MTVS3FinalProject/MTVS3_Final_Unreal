// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_BuyCoinsWidget.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/EditableText.h"
#include "Components/TextBlock.h"
#include "HJ/TTGameInstance.h"

void UMH_BuyCoinsWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_Coin_5000->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::OnClickedCoin_5000Button);
	Btn_Coin_10000->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::OnClickedCoin_10000Button);
	Btn_Coin_50000->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::OnClickedCoin_50000Button);
	Btn_Coin_100000->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::OnClickedCoin_100000Button);
	Btn_CustomCoin->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::OnClickedCoin_freeButton);
	Btn_BuyCoin->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::OnClickedBuyCoinButton);

	//SetVisibleCanvas(false);

	ButtonArray.Add(Btn_Coin_5000);
	ButtonArray.Add(Btn_Coin_10000);
	ButtonArray.Add(Btn_Coin_50000);
	ButtonArray.Add(Btn_Coin_100000);
	ButtonArray.Add(Btn_CustomCoin);
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

void UMH_BuyCoinsWidget::OnClickedCoin_5000Button()
{
	AddCoinAmount = 5000;
	OnButtonClicked(Btn_Coin_5000);
}

void UMH_BuyCoinsWidget::OnClickedCoin_10000Button()
{
	AddCoinAmount = 10000;
	OnButtonClicked(Btn_Coin_10000);
}

void UMH_BuyCoinsWidget::OnClickedCoin_50000Button()
{
	AddCoinAmount = 50000;
	OnButtonClicked(Btn_Coin_50000);
}


void UMH_BuyCoinsWidget::OnClickedCoin_100000Button()
{
	AddCoinAmount = 100000;
	OnButtonClicked(Btn_Coin_100000);
}

void UMH_BuyCoinsWidget::SetCustomCoinAmount()
{
	//
	Text_CustomCoinAmount->SetText(EText_CustomCoinAmount->GetText());
}

void UMH_BuyCoinsWidget::OnClickedCoin_freeButton()
{
	AddCoinAmount = 100000;
	OnButtonClicked(Btn_CustomCoin);
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

void UMH_BuyCoinsWidget::OnButtonClicked(UButton* ClickedButton)
{
	// 배열에 있는 모든 버튼을 순회
	for (UButton* Button : ButtonArray)
	{
		if (Button == ClickedButton)
		{
			// 클릭된 버튼 색상 변경
			Button->SetBackgroundColor(FLinearColor(0.2f,0.6f,1.f,1.f));
		}
		else
		{
			// 나머지 버튼은 원래 색상으로 변경
			Button->SetBackgroundColor(FLinearColor::White);
		}
	}
}

//void UMH_BuyCoinsWidget::OnClickedBackButton()
//{
	//SetVisibility(ESlateVisibility::Hidden);
//}
