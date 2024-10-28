// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_GameWidget.h"

#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

void UMH_GameWidget::NativeConstruct()
{
	Super::NativeConstruct();

	
}

void UMH_GameWidget::SetWidgetSwitcher(int32 num)
{
	//서버에서 불러와서 입력
	WS_GameWidgetSwitcher->SetActiveWidgetIndex(num);
	if(num==1)
	{
		PlayRouletteAnim04();
	}
	ShowWidget();
}

void UMH_GameWidget::HideWidget()
{
	WS_GameWidgetSwitcher->SetVisibility(ESlateVisibility::Hidden);
}

void UMH_GameWidget::ShowWidget()
{
	WS_GameWidgetSwitcher->SetVisibility(ESlateVisibility::Visible);
}

void UMH_GameWidget::SetTextCurrentPlayer(int32 CurrentPlayer)
{
	//남아있는 현재 인원
	//게임모드에서? 게임룰 진행되면서 가져오기
	Text_CurrentPlayer->SetText(FText::AsNumber(CurrentPlayer));
}

void UMH_GameWidget::SetTextCountDown(FString CountDownTime)
{
	//남은시간
	Text_CountDown->SetText(FText::FromString(CountDownTime));
}

void UMH_GameWidget::SetTextroulette1(FString roulette1)
{
	//룰렛1
	Text_roulette1->SetText(FText::FromString(roulette1));
}

void UMH_GameWidget::SetTextroulette2(int32 roulette2)
{
	//룰렛2
	switch (roulette2)
	{
	case 0 :
		Text_roulette2->SetText(FText::FromString(TEXT("과(와) 같은 열만")));
		break;
		
	case 1 :
		Text_roulette2->SetText(FText::FromString(TEXT("과(와) 같은 행만")));
		break;
		
	case 2 :
		Text_roulette2->SetText(FText::FromString(TEXT("과(와) 같은 열 제외")));
		break;
		
	case 3 :
		Text_roulette2->SetText(FText::FromString(TEXT("과(와) 같은 행 제외")));
		break;

	case 4 :
		Text_roulette2->SetText(FText::FromString(TEXT("만")));
		break;
	}
	
}

void UMH_GameWidget::SetTextroulette3(int32 roulette3)
{
	//룰렛3
	if(roulette3 == 0)
	{
		Text_roulette3->SetText(FText::FromString(TEXT("통과")));
	}
	else if(roulette3 == 1)
	{
		Text_roulette3->SetText(FText::FromString(TEXT("탈락")));
	}
}

void UMH_GameWidget::PlayRouletteAnim04()
{
	Can_03->SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(Roulette04Anim);
	Can_02->SetVisibility(ESlateVisibility::Hidden);
}

//룰렛 플레이 시작
void UMH_GameWidget::PlayRouletteAnim()
{
	PlayAnimation(Roulette01Anim);
	PlayAnimation(Roulette02Anim);
	PlayAnimation(Roulette03Anim);
}

void UMH_GameWidget::SetTextMyNum(int32 MyNum)
{
	Text_MyNum->SetText(FText::FromString(FString::FromInt(MyNum)));
}

void UMH_GameWidget::SetOnlyVisibleMyNum(bool bIsVisible)
{
	if(bIsVisible)
	{
		Can_03->SetVisibility(ESlateVisibility::Visible);
		Can_04->SetVisibility(ESlateVisibility::Visible);
		Can_02->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		Can_03->SetVisibility(ESlateVisibility::Hidden);
		Can_04->SetVisibility(ESlateVisibility::Hidden);
		Can_02->SetVisibility(ESlateVisibility::Visible);
	}

	
}
//CurrentUser만 보여지게 
void UMH_GameWidget::SetCurrentPlayerVisible()
{
	Can_03->SetVisibility(ESlateVisibility::Hidden);
	Can_02->SetVisibility(ESlateVisibility::Hidden);
	Can_04->SetVisibility(ESlateVisibility::Visible);
}

