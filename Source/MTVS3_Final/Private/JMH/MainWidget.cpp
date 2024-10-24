﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MainWidget.h"

#include "AssetTypeCategories.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "HJ/TTGameInstance.h"
#include "LHM/HM_HttpActor2.h"
#include "Kismet/GameplayStatics.h"
#include <HJ/TTPlayer.h>
#include <HJ/TTPlayerState.h>

void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibleCanvas(true);

	Btn_BackMain->OnClicked.AddDynamic(this , &UMainWidget::OnClickedBackMain);
	Btn_BuyTicket->OnClicked.AddDynamic(this , &UMainWidget::OnClickedBuyTicket);
	Btn_BuyLater->OnClicked.AddDynamic(this , &UMainWidget::OnClickedBack_Map);
	Btn_FailBack->OnClicked.AddDynamic(this , &UMainWidget::OnClickedBack_Map);
	//Btn_BuyTickerBack->OnClicked.AddDynamic(this , &UMainWidget::OnClickedBack_Map);
	Btn_BuyCoinsBack2->OnClicked.AddDynamic(this , &UMainWidget::OnClickedBack_Map);
	Btn_BuyCoins->OnClicked.AddDynamic(this , &UMainWidget::OnClickedBuyCoinsButton);
	Btn_SelectConcertBack->OnClicked.AddDynamic(this , &UMainWidget::OnClickedBack_Map);
	Btn_Concert01->OnClicked.AddDynamic(this , &UMainWidget::OnClickedConcert01);


}

void UMainWidget::SetWidgetSwitcher(int32 num)
{
	WS_MainWidgetSwitcher->SetActiveWidgetIndex(num);
}

void UMainWidget::SetVisibleCanvas(bool bVisible)
{
	if ( bVisible )
	{
		Can_Main->SetVisibility(ESlateVisibility::Visible);
	}

	else if ( !bVisible )
	{
		Can_Main->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainWidget::SetTextCurrentTime(FString CurrentTime)
{
	//FString으로 변환해놓은 시간값 받아와서 표시
	Tex_CurrentTime->SetText(FText::FromString(CurrentTime));

}

void UMainWidget::OnClickedBackMain()
{
	// 방에서 퇴장하고 싶다.
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		gi->ExitSession();
		GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("ExitSession"));
	}
}

void UMainWidget::OnClickedBuyTicket()
{
	//예매진행 버튼-> ButTicket으로 
	SetWidgetSwitcher(3);
}

void UMainWidget::SetTextSeatNum1(FString SeatNum1)
{
	//좌석 구역
	Tex_SeatNum1->SetText(FText::FromString(SeatNum1));
}

void UMainWidget::SetTextSeatNum2(FString SeatNum2)
{
	//좌석 번호
	Tex_SeatNum2->SetText(FText::FromString(SeatNum2));
}

void UMainWidget::OnClickedBack_Map()
{
	//나중에 예매하기버튼-> 위젯 끄기 Map 0으로 이동
	//알림 등록하고, 알림에서 클릭시 결제 진행 가능.
	SetWidgetSwitcher(0);

	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	ULocalPlayer* Local = GetWorld()->GetFirstLocalPlayerFromController();
	ATTPlayerState* PS = Cast<ATTPlayerState>(GetWorld()->GetFirstPlayerController()->PlayerState);
	if ( !GI || !Local || !PS ) return;

	GI->SetLuckyDrawState(ELuckyDrawState::Neutral);
	PS->SetLuckyDrawSeatID("-1");
}

void UMainWidget::SetTextRemainingTicket(int32 RemainingTicket)
{
	//게임인스턴스에서 남아있는 티켓 잔여횟수 받아오기
	Text_RemainingTickets->SetText(FText::AsNumber(RemainingTicket));
}

void UMainWidget::OnClickedBuyCoinsButton()
{
	SetWidgetSwitcher(4);
}

void UMainWidget::OnClickedConcert01()
{

	// KHJ: EPlaceState::ConcertHall로 변경
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	ULocalPlayer* Local = GetWorld()->GetFirstLocalPlayerFromController();
	if ( !GI || !Local) return;

	GI->SetPlaceState(EPlaceState::ConcertHall);

	if ( GI )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		if ( HttpActor2 )
		{
			HttpActor2->ReqPostConcertEntry(GI->GetConcertName() , GI->GetAccessToken());
		}
	}

	//현민
	//서버에서 콘서트 정보 받아오기
	//로비로이동
}
