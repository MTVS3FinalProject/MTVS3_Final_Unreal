// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_UserCoinWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "HJ/TTGameInstance.h"

void UMH_UserCoinWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		// 초기 코인 값 설정
		SetUserCoin(gi->GetCoin());
        
		// 델리게이트 구독
		gi->OnCoinChanged.AddDynamic(this, &UMH_UserCoinWidget::OnCoinValueChanged);
	}
	Btn_BuyCoins->OnClicked.AddDynamic(this,&UMH_UserCoinWidget::OnClickedBuyCoinsBtn);
}

void UMH_UserCoinWidget::SetUserCoin(int32 UserCoin)
{
	Text_UserCoin->SetText(FText::AsNumber(UserCoin));
}

void UMH_UserCoinWidget::OnCoinValueChanged(int32 NewCoin)
{
	SetUserCoin(NewCoin);
}

// 위젯이 소멸될 때 델리게이트 구독 해제를 위해 추가
void UMH_UserCoinWidget::NativeDestruct()
{
	Super::NativeDestruct();
	if (gi)
	{
		gi->OnCoinChanged.RemoveDynamic(this, &UMH_UserCoinWidget::OnCoinValueChanged);
	}
}