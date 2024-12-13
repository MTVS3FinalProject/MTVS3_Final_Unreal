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

	Btn_Coin_5000->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::OnClickedCoin_5000);
	Btn_Coin_10000->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::OnClickedCoin_10000);
	Btn_Coin_50000->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::OnClickedCoin_50000);
	Btn_Coin_100000->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::OnClickedCoin_100000);
	Btn_CustomCoin->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::OnClickedCoin_Custom);
	Btn_BuyCoin->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::OnClickedBuyCoinButton);
	Btn_Coin_5000->OnHovered.AddDynamic(this , &UMH_BuyCoinsWidget::OnHoveredCoin_5000);
	Btn_Coin_10000->OnHovered.AddDynamic(this , &UMH_BuyCoinsWidget::OnHoveredCoin_10000);
	Btn_Coin_50000->OnHovered.AddDynamic(this , &UMH_BuyCoinsWidget::OnHoveredCoin_50000);
	Btn_Coin_100000->OnHovered.AddDynamic(this , &UMH_BuyCoinsWidget::OnHoveredCoin_100000);
	Btn_CustomCoin->OnHovered.AddDynamic(this , &UMH_BuyCoinsWidget::OnHoveredCoin_Custom);
	Btn_Coin_5000->OnUnhovered.AddDynamic(this , &UMH_BuyCoinsWidget::OnUnhoveredCoin_5000);
	Btn_Coin_10000->OnUnhovered.AddDynamic(this , &UMH_BuyCoinsWidget::OnUnhoveredCoin_10000);
	Btn_Coin_50000->OnUnhovered.AddDynamic(this , &UMH_BuyCoinsWidget::OnUnhoveredCoin_50000);
	Btn_Coin_100000->OnUnhovered.AddDynamic(this , &UMH_BuyCoinsWidget::OnUnhoveredCoin_100000);
	Btn_CustomCoin->OnUnhovered.AddDynamic(this , &UMH_BuyCoinsWidget::OnUnhoveredCoin_Custom);


	//Main 닫기버튼 바인드
	Btn_Back06_2->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::CloseButtonPressed);
	//결제완료 닫기버튼 바인드
	Btn_BuyCoinWinExit->OnClicked.AddDynamic(this , &UMH_BuyCoinsWidget::CloseBtnBuyCoinWin);
	//현민 근데 결제 완료를 하면 넘어가는 7번 스위처 부분에서 코인 업데이트를 해주나? 아니면 gi에서 델리게이트로 업데이트 뿌려줘야 할듯.
	
	GetTextCurCoin();

	// 버튼과 텍스트 그룹 추가
	ButtonTextGroups.Add({Btn_Coin_5000 , {Text_5000_01 , Text_5000_02}});
	ButtonTextGroups.Add({Btn_Coin_10000 , {Text_10000_01 , Text_10000_02}});
	ButtonTextGroups.Add({Btn_Coin_50000 , {Text_50000_01 , Text_50000_02}});
	ButtonTextGroups.Add({Btn_Coin_100000 , {Text_100000_01 , Text_100000_02}});
	ButtonTextGroups.Add({Btn_CustomCoin , {Text_Custom_01 , Text_Custom_02}});

	Can_BuyCoinWin->SetVisibility(ESlateVisibility::Hidden);
	
}

void UMH_BuyCoinsWidget::SetCoin(int32 AfterCoin, int32 ToTalPayment, int32 AddCoin)
{
	GetTextCurCoin();
	//충전 후 코인
	Text_AfterCoin->SetText(FText::FromString(FString::FromInt(AfterCoin)));
	Text_TotalPayment->SetText(FText::FromString(FString::FromInt(ToTalPayment)));
	Text_AddCoin->SetText(FText::FromString(FString::FromInt(AddCoin)));
	//SetgiCoin(AddCoin);
}

//현재 코인값 불러오기
void UMH_BuyCoinsWidget::GetTextCurCoin()
{
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		Text_CurCoin->SetText(FText::FromString(FString::FromInt(gi->GetCoin())));
	}
}

void UMH_BuyCoinsWidget::GetCurCoin()
{
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		CurCoin = gi->GetCoin();
	}
}

void UMH_BuyCoinsWidget::SetgiCoin(int32 AddCoin)
{
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		//gi->AddCoin(AddCoin);
	}
}

void UMH_BuyCoinsWidget::HiddenBuyCoinWin()
{
	Can_BuyCoinWin->SetVisibility(ESlateVisibility::Hidden);
}

void UMH_BuyCoinsWidget::SetVisibleCanvas(bool bVisible)
{
	if (bVisible)
	{
		Can_Main->SetVisibility(ESlateVisibility::Visible);
	}

	else if (!bVisible)
	{
		Can_Main->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMH_BuyCoinsWidget::OnClickedCoin_5000()
{
	TotalCoin = 0;
	GetCurCoin();
	TotalCoin = CurCoin + 5000;
	SetCoin(TotalCoin, 5000, 5000);
	OnButtonClicked(Btn_Coin_5000);
}

void UMH_BuyCoinsWidget::OnHoveredCoin_5000()
{
	OnButtonOnHovered(true,Btn_Coin_5000);
}

void UMH_BuyCoinsWidget::OnUnhoveredCoin_5000()
{
	OnButtonOnHovered(false,Btn_Coin_5000);
}

void UMH_BuyCoinsWidget::OnClickedCoin_10000()
{
	TotalCoin = 0;
	GetCurCoin();
	TotalCoin = CurCoin + 10100;
	SetCoin(TotalCoin, 10000, 10100);
	OnButtonClicked(Btn_Coin_10000);
}

void UMH_BuyCoinsWidget::OnHoveredCoin_10000()
{
	OnButtonOnHovered(true,Btn_Coin_10000);
}

void UMH_BuyCoinsWidget::OnUnhoveredCoin_10000()
{
	OnButtonOnHovered(false,Btn_Coin_10000);
}

void UMH_BuyCoinsWidget::OnClickedCoin_50000()
{
	TotalCoin = 0;
	GetCurCoin();
	TotalCoin = CurCoin + 51000;
	SetCoin(TotalCoin, 50000, 51000);
	OnButtonClicked(Btn_Coin_50000);
}

void UMH_BuyCoinsWidget::OnHoveredCoin_50000()
{
	OnButtonOnHovered(true,Btn_Coin_50000);
}

void UMH_BuyCoinsWidget::OnUnhoveredCoin_50000()
{
	OnButtonOnHovered(false,Btn_Coin_50000);
}

void UMH_BuyCoinsWidget::OnClickedCoin_100000()
{
	TotalCoin = 0;
	GetCurCoin();
	TotalCoin = CurCoin + 102500;
	SetCoin(TotalCoin, 100000, 102500);
	OnButtonClicked(Btn_Coin_100000);
}

void UMH_BuyCoinsWidget::OnHoveredCoin_100000()
{
	OnButtonOnHovered(true,Btn_Coin_100000);
}

void UMH_BuyCoinsWidget::OnUnhoveredCoin_100000()
{
	OnButtonOnHovered(false,Btn_Coin_100000);
}

void UMH_BuyCoinsWidget::OnHoveredCoin_Custom()
{
	OnButtonOnHovered(true,Btn_CustomCoin);
	Text_CustomCoinAmount->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	SetEditableTextHintTextColor(EText_CustomCoinAmount, FLinearColor::White);
}

void UMH_BuyCoinsWidget::SetEditableTextHintTextColor(UEditableText* EditableText, FLinearColor NewHintTextColor)
{
	if (!EditableText) return;
	EditableText->WidgetStyle.SetColorAndOpacity(FSlateColor(NewHintTextColor));
}

void UMH_BuyCoinsWidget::OnUnhoveredCoin_Custom()
{
	OnButtonOnHovered(false,Btn_CustomCoin);
	Text_CustomCoinAmount->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
}

//여기 작업 해야함.
void UMH_BuyCoinsWidget::SetCustomCoinAmount()
{
	Text_CustomCoinAmount->SetText(EText_CustomCoinAmount->GetText().IsEmpty() ? FText::FromString(TEXT("0")) : EText_CustomCoinAmount->GetText());
}

void UMH_BuyCoinsWidget::OnClickedCoin_Custom()
{
	TotalCoin = 0;
	SetCustomCoinAmount();
	GetCurCoin();
	int32 CustomCoin = FCString::Atoi(*Text_CustomCoinAmount->GetText().ToString());
	TotalCoin = CurCoin + CustomCoin;

	// 추가 비율 초기화
	double r = 0.0;
	// 추가 비율 결정
	if (CustomCoin < 10000) {
		r = 0.0;
	} else if (CustomCoin < 50000) {
		r = 1.0;
	} else if (CustomCoin < 100000) {
		r = 2.0;
	} else {
		r = 2.5;
	}
	// 최종 충전 코인 계산
	int AddCoin = CustomCoin + static_cast<int>(CustomCoin * r / 100);
	
	SetCoin(TotalCoin, CustomCoin, AddCoin);
	
	OnButtonClicked(Btn_CustomCoin);
}

//충전하기
void UMH_BuyCoinsWidget::OnClickedBuyCoinButton()
{
	//결제연동...
	//충전
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		gi->SetCoin(TotalCoin); //코인 충전
	}

	//결제완료되었습니다 창 표시
	Can_BuyCoinWin->SetVisibility(ESlateVisibility::Visible);
}

void UMH_BuyCoinsWidget::OnButtonClicked(UButton* ClickedButton)
{
	for (const FButtonTextGroup& Group : ButtonTextGroups)
	{
		if (Group.Button == ClickedButton)
		{
			//클릭된 버튼과 텍스트 색상 변경
			Group.Button->SetBackgroundColor(FLinearColor(0.2f , 0.6f , 1.f , 1.f));
			for (UTextBlock* Text : Group.TextBlocks)
			{
				Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
			}
		}
		else
		{
			// 나머지 버튼과 텍스트 색상 원래대로
			Group.Button->SetBackgroundColor(FLinearColor::White);
			for (UTextBlock* Text : Group.TextBlocks)
			{
				Text->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
			}
		}
	}

	/*
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
	}*/
}

void UMH_BuyCoinsWidget::OnButtonOnHovered(bool bIsHovere ,UButton* OnHoveredButton)
{
	if(bIsHovere)
	{
		for (const FButtonTextGroup& Group : ButtonTextGroups)
		{
			if (Group.Button == OnHoveredButton)
			{
				//텍스트 색상 변경
				for (UTextBlock* Text : Group.TextBlocks)
				{
					Text->SetColorAndOpacity(FSlateColor(FLinearColor::White));
				}
			}
			else
			{
				// 나머지 버튼과 텍스트 색상 원래대로
				for (UTextBlock* Text : Group.TextBlocks)
				{
					Text->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
				}
			}
		}
	}
	else
	{
		for (const FButtonTextGroup& Group : ButtonTextGroups)
		{
			if (Group.Button == OnHoveredButton)
			{
				//텍스트 색상 변경
				for (UTextBlock* Text : Group.TextBlocks)
				{
					Text->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
				}
			}
			else
			{
				// 나머지 버튼과 텍스트 색상 원래대로
				for (UTextBlock* Text : Group.TextBlocks)
				{
					Text->SetColorAndOpacity(FSlateColor(FLinearColor::Black));
				}
			}
		}
	}
}

//void UMH_BuyCoinsWidget::OnClickedBackButton()
//{
//SetVisibility(ESlateVisibility::Hidden);
//}
