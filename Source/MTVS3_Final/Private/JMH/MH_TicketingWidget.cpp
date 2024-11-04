// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_TicketingWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "HJ/TTGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor2.h"
#include "JMH/MH_Chair.h"
#include "JMH/MainWidget.h"

void UMH_TicketingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Btn_Back1->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedBackButton);
	//Btn_Back2->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedBackButton);
	Btn_Confirm_Ticketting->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedConfirmButton);
	Btn_Cancel_Ticketting1->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedCancelButton);
	Btn_Cancel_Ticketting2->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedCancelButton);
	Btn_PlayerVisible->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedPlayerVisibleButton);
	Btn_Sound->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedSoundButton);
	Btn_GotoGameRoom->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedGotoGameRoomButton);

	//위젯 꺼져있는게 기본값
	//SetVisibleSwitcher(false);
	WS_RegisterSwitcher->SetVisibility(ESlateVisibility::Hidden);
	//현민 : 작업 완료후 지우기
	SetCompletedVisible(false);

	//콘서트정보 담아오기
	//SetConcertInfo();
	
	//// HttpActor2 공연장 입장 통신 테스트용
	//Btn_CancelRegisteredSeat->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedCancelRegisteredSeat);
	//Btn_MyRegisterSeat->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedMyRegisterSeat);
	//Btn_NoticeGameStart->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedNoticeGameStart);
	//Btn_GameResult->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedGameResult);
}

void UMH_TicketingWidget::SetWidgetSwitcher(int32 num)
{
	//서버에서 불러와서 입력
	WS_RegisterSwitcher->SetActiveWidgetIndex(num);
}

void UMH_TicketingWidget::SetVisibleSwitcher(bool bVisible , int index)
{
	switch (index)
	{
	case 0:
		if (bVisible)
		{
			WS_RegisterSwitcher->SetVisibility(ESlateVisibility::Visible);
			if (!bIsVisible && Can_RegisterAnim_On)
			{
				bIsVisible = true;
				WS_RegisterSwitcher->SetActiveWidgetIndex(0);
				PlayAnimation(Can_RegisterAnim_On);
				PlayAnimation(Over_Settings_On);
			}
		}

		else if (!bVisible)
		{
			if (bIsVisible && Can_RegisterAnim_Off)
			{
				bIsVisible = false;
				PlayAnimation(Can_RegisterAnim_Off);
				//타이머 써서 비지블 Hidden처리
				FTimerHandle RouletteTimerHandle;
				PlayAnimation(Over_Settings_Off);
				GetWorld()->GetTimerManager().SetTimer(RouletteTimerHandle, this, &UMH_TicketingWidget::HiddenCanvas, 1.1f, false);
			}
		}
		break;
	default:
		if (bVisible)
		{
			bIsVisible = true;
			WS_RegisterSwitcher->SetActiveWidgetIndex(1);
			WS_RegisterSwitcher->SetVisibility(ESlateVisibility::Visible);
		}
		else if (!bVisible)
		{
			bIsVisible = false;
			WS_RegisterSwitcher->SetVisibility(ESlateVisibility::Hidden);
		}
		break;
	}
}


void UMH_TicketingWidget::HiddenCanvas()
{
	//좌석 접수 UI 히든
	WS_RegisterSwitcher->SetVisibility(ESlateVisibility::Hidden);
}

void UMH_TicketingWidget::SetConcertInfo(FString ConcertName , int32 ConcertDateY , int32 ConcertDateM ,
                                         int32 ConcertDateD , FString ConcertTime)
{
	//접수화면 콘서트 정보 불러오기
	Text_ConcertName->SetText(FText::FromString(ConcertName));
	Text_ConcertDateY->SetText(FText::FromString(FString::FromInt(ConcertDateY)));
    Text_ConcertDateM->SetText(FText::FromString(FString::FromInt(ConcertDateM)));
    Text_ConcertDateD->SetText(FText::FromString(FString::FromInt(ConcertDateD)));
	Text_ConcertTime->SetText(FText::FromString(ConcertTime));
	//가격도 추가?->가격은 좌석마다 다름
}

void UMH_TicketingWidget::SetTextSeatID(int32 SeatFloor , FString SeatID)
{
	//좌석 층
	Text_SeatFloor->SetText(FText::AsNumber(SeatFloor));
	//좌석번호 세팅
	Text_SeatID->SetText(FText::FromString(SeatID));
	Text_SeatInfo->SetText(FText::FromString(SeatID));
}

void UMH_TicketingWidget::SetTickettingDate(int32 TickettingDateY , int32 TickettingDateM , int32 TickettingDateD)
{
	//좌석 추첨 날짜 정보
	Text_TickettingDateY->SetText(FText::FromString(FString::FromInt(TickettingDateY)));
	Text_TickettingDateM->SetText(FText::AsNumber(TickettingDateM));
	Text_TickettingDateD->SetText(FText::AsNumber(TickettingDateD));
}

void UMH_TicketingWidget::SetTextGameStartTime(FString CompetitionStartTime)
{
	//좌석 추첨 시간
	//FString으로 변환해놓은 시간값 받아와서 표시
	Text_GameStartTime->SetText(FText::FromString(CompetitionStartTime));
}

void UMH_TicketingWidget::SetTextRemainingTicket(int32 RemainingTicket)
{
	//int 가격 Ftext로 변환해서 티켓가격 입력하기
	Text_RemainingTickets->SetText(FText::AsNumber(RemainingTicket));
}

void UMH_TicketingWidget::SetCompletedVisible(bool bVisible)
{
	//텍스트, 접수신청 버튼같이 꺼주기
	if ( bVisible )
	{
		Ver_Completed->SetVisibility(ESlateVisibility::Visible);
		Btn_Cancel_Ticketting1->SetVisibility(ESlateVisibility::Visible);
		Btn_Confirm_Ticketting->SetVisibility(ESlateVisibility::Hidden);
	}

	else if ( !bVisible )
	{
		Ver_Completed->SetVisibility(ESlateVisibility::Hidden);
		Btn_Cancel_Ticketting1->SetVisibility(ESlateVisibility::Hidden);
		Btn_Confirm_Ticketting->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMH_TicketingWidget::OnClickedBackButton()
{
	//뒤로가기 (위젯 숨기기 애님 플레이)
	SetVisibleSwitcher(false, 0);
}

void UMH_TicketingWidget::OnClickedConfirmButton()
{
	//접수 클릭 버튼
	
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		AMH_Chair* Chair = Cast<AMH_Chair>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AMH_Chair::StaticClass()));
		if ( HttpActor2 && Chair )
		{
			// Chair의 태그를 가져와서 매개변수로 넘김
			FString ChairTag = Chair->Tags.Num() > 0 ? Chair->Tags[0].ToString() : FString();

			UE_LOG(LogTemp , Log , TEXT("ChairTag : %s") , *ChairTag);

			HttpActor2->ReqPostRegisterSeat(gi->GetConcertName() , ChairTag , gi->GetAccessToken());
			//접수신청버튼 안보이게, 접수완료 text,접수취소 버튼 보이게
			//SetCompletedVisible(true); 응답 완료했을 때 호출하게 해줬음
		}
	}
	//접수오류가 생기는 경우도 있나?
	//경고창 띄우기
}

void UMH_TicketingWidget::OnClickedCancelButton()
{
	//서버-> 접수취소
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		AMH_Chair* Chair = Cast<AMH_Chair>(
					UGameplayStatics::GetActorOfClass(GetWorld() , AMH_Chair::StaticClass()));
		if ( HttpActor2 && Chair )
		{
			// Chair의 태그를 가져와서 매개변수로 넘김
			FString ChairTag = Chair->Tags.Num() > 0 ? Chair->Tags[0].ToString() : FString();

			UE_LOG(LogTemp , Log , TEXT("ChairTag : %s") , *ChairTag);

			HttpActor2->ReqDeleteCancelRegisteredSeat(gi->GetConcertName() , ChairTag , gi->GetAccessToken());
			//접수신청버튼 보여지게 접수완료 text,접수취소 버튼 안보이게
			//SetCompletedVisible(false);
		}
	}
}

void UMH_TicketingWidget::SetTextGameCountDown(FString GameCountDown)
{
	Text_GameCountDown->SetText(FText::FromString(GameCountDown));
}

void UMH_TicketingWidget::SetTextTicketPrice(int32 TicketPrice)
{
	//int 가격 Ftext로 변환해서 티켓가격 입력하기
	Text_TicketPrice->SetText(FText::AsNumber(TicketPrice));
	//Text_TicketPrice2->SetText(FText::AsNumber(TicketPrice));
}

void UMH_TicketingWidget::SetSeatInfoMap(FString SeatID)
{
	//좌석별 불켜진 이미지 어떻게 만들지 이것도 이미지가 아니라
	//Widget으로 리스트 불러와서 가능한지 켜주고
	//해당 좌석에 접근햇을 때 그 좌석만 불켜주기?
	//아님 걍 이미지로 넣어두고 좌석정보에 맞춰 띄우기
}


void UMH_TicketingWidget::SetTextTicketingDeadline(FString TicketingDeadline)
{
	//FString으로 변환해놓은 시간값 받아와서 표시
	Text_TicketingDeadline->SetText(FText::FromString(TicketingDeadline));
}


void UMH_TicketingWidget::SetTextCompetitionRate(int32 CompetitionRate)
{
	//int 경쟁률 Ftext로 변환해서 경쟁률 입력하기
	Text_CompetitionRate->SetText(FText::AsNumber(CompetitionRate));
}

void UMH_TicketingWidget::OnClickedGotoGameRoomButton()
{
	//게임맵으로 세션이동
	// KHJ
	GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("SwitchSessionToLuckyDraw"));
	// 게임 인스턴스를 가져와서
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		gi->SwitchSession(EPlaceState::LuckyDrawRoom);
	}
}

void UMH_TicketingWidget::OnClickedPlayerVisibleButton()
{
	//SetPlayerVisible()
}

void UMH_TicketingWidget::SetPlayerVisible(bool bVisible)
{
	//좌석 카메라에서 플레이어 보이게, 안보이게

}

void UMH_TicketingWidget::OnClickedSoundButton()
{
	//SetSound
}

void UMH_TicketingWidget::SetSound(bool bIsSoundOn)
{
	//소리 들리게, 안들리게.

}

/*
void UMH_TicketingWidget::OnClickedCancelRegisteredSeat()
{
	//서버-> 접수취소
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		AMH_Chair* Chair = Cast<AMH_Chair>(
					UGameplayStatics::GetActorOfClass(GetWorld() , AMH_Chair::StaticClass()));
		if ( HttpActor2 && Chair )
		{
			// Chair의 태그를 가져와서 매개변수로 넘김
			FString ChairTag = Chair->Tags.Num() > 0 ? Chair->Tags[0].ToString() : FString();

			UE_LOG(LogTemp , Log , TEXT("ChairTag : %s") , *ChairTag);

			HttpActor2->ReqDeleteCancelRegisteredSeat(gi->GetConcertName() , ChairTag , gi->GetAccessToken());
		}
	}
}
void UMH_TicketingWidget::OnClickedMyRegisterSeat()
{
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		if ( HttpActor2 )
		{
			HttpActor2->ReqPostMyRegisteredSeat(gi->GetConcertName() , gi->GetAccessToken());
		}
	}
}

void UMH_TicketingWidget::OnClickedNoticeGameStart()
{
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		AMH_Chair* Chair = Cast<AMH_Chair>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AMH_Chair::StaticClass()));
		if ( HttpActor2 && Chair )
		{
			// Chair의 태그를 가져와서 매개변수로 넘김
			FString ChairTag = Chair->Tags.Num() > 0 ? Chair->Tags[0].ToString() : FString();

			UE_LOG(LogTemp , Log , TEXT("ChairTag : %s") , *ChairTag);

			HttpActor2->ReqPostNoticeGameStart(gi->GetConcertName() , ChairTag , gi->GetAccessToken());
		}
	}
}

void UMH_TicketingWidget::OnClickedGameResult()
{
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		AMH_Chair* Chair = Cast<AMH_Chair>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AMH_Chair::StaticClass()));
		if ( HttpActor2 && Chair )
		{
			// Chair의 태그를 가져와서 매개변수로 넘김
			FString ChairTag = Chair->Tags.Num() > 0 ? Chair->Tags[0].ToString() : FString();

			UE_LOG(LogTemp , Log , TEXT("ChairTag : %s") , *ChairTag);

			HttpActor2->ReqPostGameResult(gi->GetConcertName() , ChairTag , gi->GetAccessToken());
		}
	}
}*/

