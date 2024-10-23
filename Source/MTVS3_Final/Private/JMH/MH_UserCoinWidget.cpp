// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_UserCoinWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "HJ/TTGameInstance.h"

void UMH_UserCoinWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		SetUserCoin(gi->GetCoin());
	}
}

void UMH_UserCoinWidget::SetUserCoin(int32 UserCoin)
{
	Text_UserCoin->SetText(FText::AsNumber(UserCoin));
}
