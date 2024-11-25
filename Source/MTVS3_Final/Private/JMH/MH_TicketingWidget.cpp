// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_TicketingWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
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
	//Btn_Back1_1->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedBack1_1);
	//Btn_Back2->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedBackButton);
	Btn_Confirm_Ticketting->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedConfirmButton);
	Btn_Cancel_Ticketting1->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedCancelButton);
	Btn_Cancel_Ticketting2->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedCancelButton2);
	Btn_PlayerVisible->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedPlayerVisibleButton);
	Btn_Sound->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedSoundButton);
	Btn_GotoGameRoom->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedGotoGameRoomButton);

	//위젯 꺼져있는게 기본값
	//SetVisibleSwitcher(false);
	WS_RegisterSwitcher->SetVisibility(ESlateVisibility::Hidden);
	//현민 : 작업 완료후 지우기
	SetCompletedVisible(false);

	// KHJ
	Img_Loading->SetVisibility(ESlateVisibility::Hidden);

	//콘서트정보 담아오기
	//SetConcertInfo();

	//// HttpActor2 공연장 입장 통신 테스트용
	//Btn_CancelRegisteredSeat->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedCancelRegisteredSeat);
	//Btn_MyRegisterSeat->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedMyRegisterSeat);
	//Btn_NoticeGameStart->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedNoticeGameStart);
	//Btn_GameResult->OnClicked.AddDynamic(this , &UMH_TicketingWidget::OnClickedGameResult);

	InitializeSeatsUI();
}

void UMH_TicketingWidget::SetWidgetSwitcher(int32 num)
{
	if (!WS_RegisterSwitcher) return;
	//서버에서 불러와서 입력
	WS_RegisterSwitcher->SetActiveWidgetIndex(num);
}

void UMH_TicketingWidget::SetVisibleSwitcher(bool bVisible , int index)
{
	if (!WS_RegisterSwitcher) return;
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
				GetWorld()->GetTimerManager().SetTimer(RouletteTimerHandle , this , &UMH_TicketingWidget::HiddenCanvas ,
				                                       1.1f , false);
			}
		}
		break;

	case 1:
		if (bVisible)
		{
			bIsVisible = true;
			WS_RegisterSwitcher->SetActiveWidgetIndex(1);
			WS_RegisterSwitcher->SetVisibility(ESlateVisibility::Visible);
			//추첨장 입장 비지블온
			SetBattleEntryVisible(true);

			ULocalPlayer* Local = GetWorld()->GetFirstLocalPlayerFromController();
			if (Local)
			{
				APlayerController* PC = Local->GetPlayerController(GetWorld());
				if (PC)
				{
					ATTPlayer* TTPlayer = Cast<ATTPlayer>(PC->GetPawn());
					if (TTPlayer && TTPlayer->GetbIsHost() == false)
					{
						PC->SetInputMode(FInputModeUIOnly());
					}
				}
			}
		}

		else if (!bVisible)
		{
			bIsVisible = false;
			WS_RegisterSwitcher->SetVisibility(ESlateVisibility::Hidden);
			//추첨장 입장 비지블오프
			SetBattleEntryVisible(false);

			ULocalPlayer* Local = GetWorld()->GetFirstLocalPlayerFromController();
			if (Local)
			{
				APlayerController* PC = Local->GetPlayerController(GetWorld());
				if (PC)
				{
					ATTPlayer* TTPlayer = Cast<ATTPlayer>(PC->GetPawn());
					if (TTPlayer && TTPlayer->GetbIsHost() == false)
					{
						PC->SetInputMode(FInputModeGameAndUI());
					}
				}
			}
		}

		break;

	default:
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
	if (bVisible)
	{
		Ver_Completed->SetVisibility(ESlateVisibility::Visible);
		Btn_Cancel_Ticketting1->SetVisibility(ESlateVisibility::Visible);
		Btn_Confirm_Ticketting->SetVisibility(ESlateVisibility::Hidden);
	}

	else if (!bVisible)
	{
		Ver_Completed->SetVisibility(ESlateVisibility::Hidden);
		Btn_Cancel_Ticketting1->SetVisibility(ESlateVisibility::Hidden);
		Btn_Confirm_Ticketting->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMH_TicketingWidget::OnClickedBackButton()
{
	//뒤로가기 (위젯 숨기기 애님 플레이)
	SetVisibleSwitcher(false , 0);
}

void UMH_TicketingWidget::OnClickedConfirmButton()
{
	//접수 클릭 버튼

	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		// 플레이어 캐릭터를 가져옵니다.
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld() , 0);
		if (PlayerController)
		{
			ATTPlayer* Player = Cast<ATTPlayer>(PlayerController->GetPawn());
			if (Player)
			{
				// 플레이어에서 GetOverlappingActor 호출
				AMH_Chair* Chair = Cast<AMH_Chair>(Player->GetOverlappingActor());
				if (HttpActor2 && Chair)
				{
					// Chair의 태그를 가져와서 매개변수로 넘김
					FString ChairTag = Chair->Tags.Num() > 0 ? Chair->Tags[0].ToString() : FString();

					UE_LOG(LogTemp , Log , TEXT("ChairTag : %s") , *ChairTag);

					HttpActor2->ReqGetRegisterSeat(ChairTag , gi->GetAccessToken());
					//접수신청버튼 안보이게, 접수완료 text,접수취소 버튼 보이게
					//SetCompletedVisible(true); 응답 완료했을 때 호출하게 해줬음
				}
			}
		}
	}
	//접수오류가 생기는 경우도 있나?
	//경고창 띄우기
}

void UMH_TicketingWidget::OnClickedCancelButton()
{
	//서버-> 접수취소
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		// 플레이어 캐릭터를 가져옵니다.
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld() , 0);
		if (PlayerController)
		{
			ATTPlayer* Player = Cast<ATTPlayer>(PlayerController->GetPawn());
			if (Player)
			{
				// 플레이어에서 GetOverlappingActor 호출
				AMH_Chair* Chair = Cast<AMH_Chair>(Player->GetOverlappingActor());
				if (HttpActor2 && Chair)
				{
					// Chair의 태그를 가져와서 매개변수로 넘김
					FString ChairTag = Chair->Tags.Num() > 0 ? Chair->Tags[0].ToString() : FString();

					UE_LOG(LogTemp , Log , TEXT("ChairTag : %s") , *ChairTag);

					HttpActor2->ReqDeleteCancelRegisteredSeat(ChairTag , gi->GetAccessToken());
					//접수신청버튼 보여지게 접수완료 text,접수취소 버튼 안보이게
					//SetCompletedVisible(false);
				}
			}
		}
	}
}

void UMH_TicketingWidget::OnClickedCancelButton2()
{
	//서버-> 접수취소
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		if (HttpActor2)
		{
			HttpActor2->ReqDeleteCancelRegisteredSeat2(gi->GetReceivedSeatId() , gi->GetAccessToken());
		}
	}
}

//void UMH_TicketingWidget::OnClickedBack1_1()
//{
//	SetVisibleSwitcher(false,1);
//}

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

void UMH_TicketingWidget::SetLoadingActive(bool bIsActive)
{
	if (bIsActive == true)
	{
		Img_Loading->SetVisibility(ESlateVisibility::Visible);
		PlayAnimation(LoadingAnim);
	}
}

void UMH_TicketingWidget::OnClickedGotoGameRoomButton()
{
	//게임맵으로 세션이동
	// KHJ
	GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("SwitchSessionToLuckyDraw"));
	// 게임 인스턴스를 가져와서
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if (gi)
	{
		gi->SwitchSession(EPlaceState::LuckyDrawRoom);
	}

	SetLoadingActive(true);
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

//추첨장 입장 UI띄우기 비지블로 변경
void UMH_TicketingWidget::SetBattleEntryVisible(bool bVisible)
{
	if (bVisible)
	{
		Can_TicketBattleEntry->SetVisibility(ESlateVisibility::Visible);
	}

	else if (!bVisible)
	{
		Can_TicketBattleEntry->SetVisibility(ESlateVisibility::Hidden);
	}
}

//좌석별 불켜진 이미지 어떻게 만들지 이것도 이미지가 아니라
//Widget으로 리스트 불러와서 가능한지 켜주고
//해당 좌석에 접근햇을 때 그 좌석만 불켜주기?
//아님 걍 이미지로 넣어두고 좌석정보에 맞춰 띄우기

//좌석 상태표시 UI 기능
void UMH_TicketingWidget::InitializeSeatsUI()
{
	//좌석정보 초기화
	// SeatOverlays 배열 초기화 (오버레이를 수동으로 추가해야 함)
	for (int32 i = 1; i < 66; i++)
	{
		// SeatOverlays에 오버레이 위젯 추가 (디자이너에서 이름으로 바인딩)
		FString OverlayName = FString::Printf(TEXT("Over_ASeat%d") , i);
		UOverlay* Overlay = Cast<UOverlay>(GetWidgetFromName(FName(*OverlayName)));
		if (Overlay)
		{
			SeatOverlays.Add(Overlay);
		}
	}

	// SeatStates 초기화
	SeatStates.Init(false , 65); // 기본값: 접수 가능석 비활성화
}

void UMH_TicketingWidget::SetCurrentSelectedSeatUI(FString ChairTag)
{
	for (UOverlay* Overlay : SeatOverlays)
	{
		if (Overlay && Overlay->GetChildrenCount() > 1) // 두 번째 이미지가 선택석
		{
			UImage* SelectedImage = Cast<UImage>(Overlay->GetChildAt(1));
			if (SelectedImage)
			{
				SelectedImage->SetVisibility(ESlateVisibility::Hidden);
			}
		}
	}
	
	int32 SeatIndex = FCString::Atoi(*ChairTag) - 1;

	if (SeatIndex >= 0 && SeatIndex < SeatStates.Num())
	{
		//현재 선택석 설정 함수
		if (SeatOverlays.IsValidIndex(SeatIndex))
		{
			// 오버레이의 첫 번째 이미지를 활성화 (현재 선택석)
			UOverlay* Overlay = SeatOverlays[SeatIndex];
			if (Overlay && Overlay->GetChildrenCount() > 0)
			{
				// 자식 이미지 위젯 가져오기
				UImage* SelectedImage = Cast<UImage>(Overlay->GetChildAt(1));
				if (SelectedImage)
				{
					SelectedImage->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}
	}
}

void UMH_TicketingWidget::UpdateReservedSeatsUI(const TArray<int32> ReservedSeats)
{
	//접수 완료 업데이트
	// 모든 좌석 초기화
	for (int32 i = 0; i < SeatOverlays.Num(); i++)
	{
		if (SeatOverlays[i])
		{
			// 모든 이미지를 비활성화
			for (int32 j = 0; j < SeatOverlays[i]->GetChildrenCount(); j++)
			{
				UImage* Image = Cast<UImage>(SeatOverlays[i]->GetChildAt(j));
				if (Image)
				{
					Image->SetVisibility(ESlateVisibility::Hidden);
				}
			}
		}
	}

	// 접수 완료석 업데이트
	for (int32 SeatIndex : ReservedSeats)
	{
		if (SeatOverlays.IsValidIndex(SeatIndex))
		{
			UOverlay* Overlay = SeatOverlays[SeatIndex];
			if (Overlay && Overlay->GetChildrenCount() > 1) // 첫 번째 이미지: 접수 완료석
			{
				UImage* ReservedImage = Cast<UImage>(Overlay->GetChildAt(0));
				if (ReservedImage)
				{
					ReservedImage->SetVisibility(ESlateVisibility::Visible);
				}
			}
		}
	}
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
