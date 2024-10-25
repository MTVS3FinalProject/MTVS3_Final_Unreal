// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_GameWidget.h"

#include "Components/Button.h"
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
	Tex_CurrentPlayer->SetText(FText::AsNumber(CurrentPlayer));
}

void UMH_GameWidget::SetTextCountDown(FString CountDownTime)
{
	//남은시간
	Tex_CountDown->SetText(FText::FromString(CountDownTime));
}

void UMH_GameWidget::SetTextroulette1(FString roulette1)
{
	//룰렛1
	Tex_roulette1->SetText(FText::FromString(roulette1));
}

void UMH_GameWidget::SetTextroulette2(int32 roulette2)
{
	//룰렛2
	switch (roulette2)
	{
	case 0 :
		Tex_roulette2->SetText(FText::FromString(TEXT("과(와) 같은 열만")));
		break;
		
	case 1 :
		Tex_roulette2->SetText(FText::FromString(TEXT("과(와) 같은 행만")));
		break;
		
	case 2 :
		Tex_roulette2->SetText(FText::FromString(TEXT("과(와) 같은 열 제외")));
		break;
		
	case 3 :
		Tex_roulette2->SetText(FText::FromString(TEXT("과(와) 같은 행 제외")));
		break;

	case 4 :
		Tex_roulette2->SetText(FText::FromString(TEXT("만")));
		break;
	}
	
}

void UMH_GameWidget::SetTextroulette3(int32 roulette3)
{
	//룰렛3
	if(roulette3 == 0)
	{
		Tex_roulette3->SetText(FText::FromString(TEXT("통과")));
	}
	else if(roulette3 == 1)
	{
		Tex_roulette3->SetText(FText::FromString(TEXT("탈락")));
	}
}

//룰렛 플레이 시작
void UMH_GameWidget::PlayRouletteAnim()
{
	PlayAnimation(Roulette01Anim);
	PlayAnimation(Roulette02Anim);
	PlayAnimation(Roulette03Anim);
}

