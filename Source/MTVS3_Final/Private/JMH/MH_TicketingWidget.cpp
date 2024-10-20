﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_TicketingWidget.h"

#include "Components/Button.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/WidgetSwitcher.h"
#include "HJ/TTGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor2.h"

void UMH_TicketingWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Btn_Back1->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedBackButton);
	Btn_Back2->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedBackButton);
	Btn_Confirm_Ticketting->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedConfirmButton);
	Btn_Cancel_Ticketting1->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedCancelButton);
	Btn_Cancel_Ticketting2->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedCancelButton);
	Btn_PlayerVisible->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedPlayerVisibleButton);
	Btn_Sound->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedSoundButton);
	Btn_GotoGameRoom->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedGotoGameRoomButton);
	//현민 test-> 나중에 각자 제자리에 복붙하기
	btn_test1->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedtest1);
	btn_test2->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedtest2);
	btn_test3->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedtest3);
	btn_test4->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedtest4);
	btn_test5->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedtest5);
	btn_test6->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedtest6);
	btn_test7->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedtest7);
	btn_test8->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedtest8);
	btn_test9->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedtest9);

	//위젯 꺼져있는게 기본값
	SetVisibleSwitcher(false);
	SetCompletedVisible(false);

	//콘서트정보 담아오기
	//SetConcertInfo();
}

void UMH_TicketingWidget::SetWidgetSwitcher(int32 num)
{
	//서버에서 불러와서 입력
	WS_RegisterSwitcher->SetActiveWidgetIndex(num);
}

void UMH_TicketingWidget::SetVisibleSwitcher(bool bVisible)
{
	if ( bVisible )
	{
		WS_RegisterSwitcher->SetVisibility(ESlateVisibility::Visible);
	}

	else if ( !bVisible )
	{
		WS_RegisterSwitcher->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMH_TicketingWidget::SetConcertInfo(FString ConcertName , int32 ConcertDateY , int32 ConcertDateM ,
										 int32 ConcertDateD , FString ConcertTime)
{
	//접수화면 콘서트 정보 불러오기
	Text_ConcertName->SetText(FText::FromString(ConcertName));
	Text_ConcertDateY->SetText(FText::AsNumber(ConcertDateY));
	Text_ConcertDateM->SetText(FText::AsNumber(ConcertDateM));
	Text_ConcertDateD->SetText(FText::AsNumber(ConcertDateD));
	Text_ConcertTime->SetText(FText::FromString(ConcertTime));
	//가격도 추가?->가격은 좌석마다 다름
}

void UMH_TicketingWidget::SetTextSeatID(int32 SeatFloor , FString SeatID)
{
	//좌석 층
	Text_SeatFloor->SetText(FText::AsNumber(SeatFloor));
	//좌석번호 세팅
	Text_SeatID->SetText(FText::FromString(SeatID));
}

void UMH_TicketingWidget::SetTickettingDate(int32 TickettingDateY , int32 TickettingDateM , int32 TickettingDateD)
{
	//좌석 추첨 날짜 정보
	Text_TickettingDateY->SetText(FText::AsNumber(TickettingDateY));
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
	if ( Can_RegisterAnim_Off )
	{
		PlayAnimation(Can_RegisterAnim_Off);
	}
}

void UMH_TicketingWidget::OnClickedConfirmButton()
{
	//접수완료

	//접수신청버튼 안보이게, 접수완료 text,접수취소 버튼 보이게
	//SetCompletedVisible(true);

	//접수오류가 생기는 경우도 있나?
	//경고창 띄우기
}

void UMH_TicketingWidget::OnClickedCancelButton()
{
	//접수신청버튼 보여지게 접수완료 text,접수취소 버튼 안보이게
	SetCompletedVisible(false);

	//서버-> 접수취소

}

void UMH_TicketingWidget::SetTextTicketPrice(int32 TicketPrice)
{
	//int 가격 Ftext로 변환해서 티켓가격 입력하기
	Text_TicketPrice->SetText(FText::AsNumber(TicketPrice));
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
		gi->SwitchSessionToLuckyDraw();
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


//현민 test

//1.좌석조회요청
void UMH_TicketingWidget::OnClickedtest1()
{
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
	UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		if ( HttpActor2 )
		{
			// AvailableSeats 배열을 가져옴
			const TArray<FSeatIdDTO>& AvailableSeats = HttpActor2->GetAvailableSeats();

			// AvailableSeats의 첫 번째 요소가 있는지 확인
			if ( AvailableSeats.Num() > 0 )
			{
				// 통신 테스트용 : 첫 번째 좌석 ID를 가져옴
				// 프로토 때 : 액터의 (좌석넘버) 태그를 가져와야 함
				FString FirstAvailableSeatId = AvailableSeats[0].GetSeatId();
				UE_LOG(LogTemp , Log , TEXT("AvailableSeats[0] : %s"), *FirstAvailableSeatId);

				HttpActor2->ReqPostSeatRegistrationInquiry(HttpActor2->GetConcertName() , FirstAvailableSeatId , gi->GetAccessToken());
			}
		}
	}
}

//2.좌석접수요청
void UMH_TicketingWidget::OnClickedtest2()
{
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
	UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		if ( HttpActor2 )
		{
			// AvailableSeats 배열을 가져옴
			const TArray<FSeatIdDTO>& AvailableSeats = HttpActor2->GetAvailableSeats();

			// AvailableSeats의 첫 번째 요소가 있는지 확인
			if ( AvailableSeats.Num() > 0 )
			{
				// 통신 테스트용 : 첫 번째 좌석 ID를 가져옴
				// 프로토 때 : 액터의 (좌석넘버) 태그를 가져와야 함
				FString FirstAvailableSeatId = AvailableSeats[0].GetSeatId();
				UE_LOG(LogTemp , Log , TEXT("AvailableSeats[0] : %s") , *FirstAvailableSeatId);

				HttpActor2->ReqPostRegisterSeat(HttpActor2->GetConcertName() , FirstAvailableSeatId , gi->GetAccessToken());
			}
		}
	}
}
//3.좌석취소요청
void UMH_TicketingWidget::OnClickedtest3()
{
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
	UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		if ( HttpActor2 )
		{
			// AvailableSeats 배열을 가져옴
			const TArray<FSeatIdDTO>& AvailableSeats = HttpActor2->GetAvailableSeats();

			// AvailableSeats의 첫 번째 요소가 있는지 확인
			if ( AvailableSeats.Num() > 0 )
			{
				// 통신 테스트용 : 첫 번째 좌석 ID를 가져옴
				// 프로토 때 : 액터의 (좌석넘버) 태그를 가져와야 함
				FString FirstAvailableSeatId = AvailableSeats[0].GetSeatId();
				UE_LOG(LogTemp , Log , TEXT("AvailableSeats[0] : %s") , *FirstAvailableSeatId);

				HttpActor2->ReqDeleteCancelRegisteredSeat(HttpActor2->GetConcertName() , FirstAvailableSeatId , gi->GetAccessToken());
			}
		}
	}
}
//4.게임결과요청
void UMH_TicketingWidget::OnClickedtest4()
{
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
	UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		if ( HttpActor2 )
		{
			// AvailableSeats 배열을 가져옴
			const TArray<FSeatIdDTO>& AvailableSeats = HttpActor2->GetAvailableSeats();

			// AvailableSeats의 첫 번째 요소가 있는지 확인
			if ( AvailableSeats.Num() > 0 )
			{
				// 통신 테스트용 : 첫 번째 좌석 ID를 가져옴
				// 프로토 때 : 액터의 (좌석넘버) 태그를 가져와야 함
				FString FirstAvailableSeatId = AvailableSeats[0].GetSeatId();
				UE_LOG(LogTemp , Log , TEXT("AvailableSeats[0] : %s") , *FirstAvailableSeatId);

				HttpActor2->ReqPostGameResult(HttpActor2->GetConcertName(), FirstAvailableSeatId , gi->GetAccessToken());
			}
		}
	}
}
//5.회원 인증용 QR요청
void UMH_TicketingWidget::OnClickedtest5()
{
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
	UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		if ( HttpActor2 )
		{
			HttpActor2->ReqGetMemberAuthQR(gi->GetAccessToken());
		}
	}
}
//6.회원사진 업로드확인 요청
void UMH_TicketingWidget::OnClickedtest6()
{
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
	UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		if ( HttpActor2 )
		{
			HttpActor2->ReqGetPostConfirmMemberPhoto(HttpActor2->GetUserCode() , gi->GetAccessToken());
		}
	}
}
//7.예매자 정보입력요청
void UMH_TicketingWidget::OnClickedtest7()
{
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
	UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		if ( HttpActor2 )
		{
			HttpActor2->ReqPostReservationinfo(HttpActor2->GetUserName() , HttpActor2->GetUserPhoneNumber() , HttpActor2->GetUserAddress() , gi->GetAccessToken());
		}
	}
}
//8.좌석 결제요청
void UMH_TicketingWidget::OnClickedtest8()
{
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
	UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		if ( HttpActor2 )
		{
			// AvailableSeats 배열을 가져옴
			const TArray<FSeatIdDTO>& AvailableSeats = HttpActor2->GetAvailableSeats();

			// AvailableSeats의 첫 번째 요소가 있는지 확인
			if ( AvailableSeats.Num() > 0 )
			{
				// 통신 테스트용 : 첫 번째 좌석 ID를 가져옴
				// 프로토 때 : 액터의 (좌석넘버) 태그를 가져와야 함
				FString FirstAvailableSeatId = AvailableSeats[0].GetSeatId();
				UE_LOG(LogTemp , Log , TEXT("AvailableSeats[0] : %s") , *FirstAvailableSeatId);

				HttpActor2->ReqPostPaymentSeat(HttpActor2->GetConcertName() , FirstAvailableSeatId , gi->GetAccessToken());
			}
		}
	}
}

void UMH_TicketingWidget::OnClickedtest9()
{
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		if ( HttpActor2 )
		{	
			HttpActor2->ReqPostConcertEntry(HttpActor2->GetConcertName() , gi->GetAccessToken());
			UE_LOG(LogTemp , Log , TEXT("HttpActor2->GetConcertName() : %s"), *HttpActor2->GetConcertName());
		}
	}
}
