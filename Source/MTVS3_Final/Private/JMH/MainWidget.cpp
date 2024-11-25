// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MainWidget.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "HJ/TTGameInstance.h"
#include "LHM/HM_HttpActor2.h"
#include "Kismet/GameplayStatics.h"
#include <HJ/TTPlayerState.h>

#include "Components/Image.h"
#include "HJ/HallSoundManager.h"
#include "HJ/TTPlayer.h"
#include "JMH/MH_Chatting.h"
#include "JMH/MH_NoticeWidget.h"
#include "LHM/HM_FinalTicket.h"
#include "LHM/HM_HttpActor3.h"
#include "LHM/HM_TicketCustom.h"


void UMainWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibleCanvas(true);

	Btn_BackMain->OnClicked.AddDynamic(this , &UMainWidget::OnClickedBackMain);
	Btn_BuyTicket->OnClicked.AddDynamic(this , &UMainWidget::OnClickedBuyTicket);
	Btn_BuyLater->OnClicked.AddDynamic(this , &UMainWidget::OnClickedBack_Map);
	Btn_FailBack->OnClicked.AddDynamic(this , &UMainWidget::OnClickedBack_Map);
	Btn_BuyCoins->OnClicked.AddDynamic(this , &UMainWidget::OnClickedBuyCoinsButton);
	Btn_SelectConcertBack->OnClicked.AddDynamic(this , &UMainWidget::OnClickedBack_Map);
	Btn_Concert01->OnClicked.AddDynamic(this , &UMainWidget::OnClickedConcert01);
	Btn_Concert02->OnClicked.AddDynamic(this , &UMainWidget::OnClickedConcert02);
	Btn_Concert03->OnClicked.AddDynamic(this , &UMainWidget::OnClickedConcert03);
	Btn_Concert04->OnClicked.AddDynamic(this , &UMainWidget::OnClickedConcert04);
	Btn_Concert05->OnClicked.AddDynamic(this , &UMainWidget::OnClickedConcert05);
	Btn_ExitMainWin->OnClicked.AddDynamic(this , &UMainWidget::OnClickedExitMainWin);
	Btn_ExitMain->OnClicked.AddDynamic(this , &UMainWidget::OnClickedExit);
	Btn_BackMainWin->OnClicked.AddDynamic(this , &UMainWidget::OnTicketWidgetClose);
	Btn_ConcertR->OnClicked.AddDynamic(this , &UMainWidget::OnClickedConcertR);
	Btn_ConcertL->OnClicked.AddDynamic(this , &UMainWidget::OnClickedConcertL);
	//Btn_BuyCoinsBack2->OnClicked.AddDynamic(this , &UMainWidget::OnClickedBack_Map);

	Btn_TutorialStart->OnClicked.AddDynamic(this , &UMainWidget::OnClickedTutorialStart);
	Btn_TutorialSkip->OnClicked.AddDynamic(this , &UMainWidget::OnClickedTutorialSkip);
	Btn_Right1->OnClicked.AddDynamic(this , &UMainWidget::OnClickedRight1);
	Btn_Left2->OnClicked.AddDynamic(this , &UMainWidget::OnClickedLeft2);
	Btn_Right2->OnClicked.AddDynamic(this , &UMainWidget::OnClickedRight2);
	Btn_Left3->OnClicked.AddDynamic(this , &UMainWidget::OnClickedLeft3);
	Btn_Right3->OnClicked.AddDynamic(this , &UMainWidget::OnClickedRight3);
	Btn_Left4->OnClicked.AddDynamic(this , &UMainWidget::OnClickedLeft4);
	Btn_Right4->OnClicked.AddDynamic(this , &UMainWidget::OnClickedRight4);
	Btn_Left5->OnClicked.AddDynamic(this , &UMainWidget::OnClickedLeft5);
	Btn_TutorialEnd->OnClicked.AddDynamic(this , &UMainWidget::OnClickedTutorialEnd);

	//닫기 버튼 다른 위젯 클래스와 연결 
	if (BuyTicketWidget)
	{
		BuyTicketWidget->OnClickedBuyTickerBack.AddDynamic(this , &UMainWidget::OnTicketWidgetClose);
	}

	if (BuyCoinsWidget)
	{
		BuyCoinsWidget->OnClickedBuyCoinBack.AddDynamic(this , &UMainWidget::OnTicketWidgetClose);
	}

	if (WBP_MH_MainBar && WBP_MH_MainBar->WBP_NoticeUI)
	{
		WBP_MH_MainBar->OnClickedShowChatBtn.AddDynamic(this , &UMainWidget::ShowChatUI);
		WBP_MH_MainBar->WBP_NoticeUI->OnClickedPaymentPostpone.AddDynamic(this , &UMainWidget::HandlePaymentPostpone);
	}

	if (TicketCustomWidget)
	{
		TicketCustomWidget->OnClickedTicketCustomBack.AddDynamic(this , &UMainWidget::OnTicketWidgetClose);
		TicketCustomWidget->OnClickedTicketCustomSave.AddDynamic(this , &UMainWidget::OnClickedCustomTicketSaveButton);
	}

	if (FinalTicketWidget)
	{
		FinalTicketWidget->OnClickedFinalTicketBack.AddDynamic(this , &UMainWidget::OnTicketWidgetClose);
	}

	//info Canvas TArray
	InfoCanvasPanels = {
		Can_ConcertInfo01 , Can_ConcertInfo02 , Can_ConcertInfo03 , Can_ConcertInfo04 , Can_ConcertInfo05
	};

	HideAllTitle();
}

void UMainWidget::SetWidgetSwitcher(int32 num)
{
	WS_MainWidgetSwitcher->SetActiveWidgetIndex(num);
	if (num == 2)
	{
		//PlayAnimation(TicketImgAnim01,0,0,EUMGSequencePlayMode::Reverse;
		PlayAnimation(TicketImgAnim01);
	}
}

void UMainWidget::SetVisibleCanvas(bool bVisible)
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

void UMainWidget::GoToConcertHall()
{
	// KHJ: EPlaceState::ConcertHall로 변경
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	ULocalPlayer* Local = GetWorld()->GetFirstLocalPlayerFromController();
	if (!GI || !Local) return;

	//GI->SetPlaceState(EPlaceState::ConcertHall);

	if (GI)
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		if (HttpActor2)
		{
			HttpActor2->ReqGetConcertEntry(GI->GetAccessToken());
			//HttpActor2->TESTReqPostConcertEntry( GI->GetAccessToken());
		}
	}
}

void UMainWidget::SetTextCurrentTime(FString CurrentTime)
{
	//FString으로 변환해놓은 시간값 받아와서 표시
	Tex_CurrentTime->SetText(FText::FromString(CurrentTime));
}

void UMainWidget::OnClickedBackMain()
{
	//HJ 버튼 사운드
	AHallSoundManager* HallSoundManager = Cast<AHallSoundManager>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHallSoundManager::StaticClass()));

	if (HallSoundManager)
	{
		HallSoundManager->PlayPlazaBGM();
	}
	// 로비로?
	auto* GI = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if (GI)
	{
		GI->SetPlaceState(EPlaceState::Plaza);
	}

	ULocalPlayer* Local = GetWorld()->GetFirstLocalPlayerFromController();
	if (Local)
	{
		APlayerController* PC = Local->GetPlayerController(GetWorld());
		if (PC)
		{
			ATTPlayer* TTPlayer = Cast<ATTPlayer>(PC->GetPawn());
			if (TTPlayer)
			{
				TTPlayer->ServerTeleportPlayer(false);
			}
		}
	}

	PlayTitleAnim(1);
}

void UMainWidget::OnClickedExitMainWin()
{
	//게임 종료 하시겠습니까?
	SetWidgetSwitcher(6);
}

void UMainWidget::OnClickedExit()
{
	//게임 종료
	UWorld* World = GetWorld();
	APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr;

	if (PlayerController)
	{
		UKismetSystemLibrary::QuitGame(World , PlayerController , EQuitPreference::Quit , true);
	}
}

void UMainWidget::ShowChatUI()
{
	bIsChatVisible = !bIsChatVisible;
	if (bIsChatVisible)
	{
		WBP_Chatting->SetVisibility(ESlateVisibility::Visible);
	}
	else if (!bIsChatVisible)
	{
		WBP_Chatting->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainWidget::SetMinimapImage(UTexture2D* img)
{
	Img_MinimapImg->SetBrushFromTexture(img);
}

void UMainWidget::OnClickedBuyTicket()
{
	//예매진행 버튼-> ButTicket으로
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
	if (GI && HttpActor2)
	{
		HttpActor2->ReqGetMemberAuthQR(GI->GetAccessToken());
	}
	//SetWidgetSwitcher(3); 통신 성공시 호출
}

void UMainWidget::SetTextSeatNum1(FString SeatNum1)
{
	//좌석 번호
	Tex_SeatNum1->SetText(FText::FromString(SeatNum1));
}

void UMainWidget::OnClickedBack_Map()
{
	//나중에 예매하기버튼-> 위젯 끄기 Map 0으로 이동
	//알림 등록하고, 알림에서 클릭시 결제 진행 가능.
	SetWidgetSwitcher(0);
	AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	ULocalPlayer* Local = GetWorld()->GetFirstLocalPlayerFromController();
	ATTPlayerState* PS = Cast<ATTPlayerState>(GetWorld()->GetFirstPlayerController()->PlayerState);
	if (!GI || !Local || !PS || !HttpActor3) return;

	HttpActor3->ReqPostponePaymentSeat(GI->GetAccessToken());

	GI->SetLuckyDrawState(ELuckyDrawState::Neutral);
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
	GoToConcertHall();
}

void UMainWidget::OnClickedConcert02()
{
	GoToConcertHall();
}

void UMainWidget::OnClickedConcert03()
{
	GoToConcertHall();
}

void UMainWidget::OnClickedConcert04()
{
	GoToConcertHall();
}

void UMainWidget::OnClickedConcert05()
{
	GoToConcertHall();
}

void UMainWidget::OnClickedConfirm_Concert()
{
	//일단 그냥 뉴진스 공연장으로 간닷
	GoToConcertHall();
}

void UMainWidget::OnTicketWidgetClose()
{
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (GI->GetbIsNewPlayer() == false) SetWidgetSwitcher(0);
	else SetWidgetSwitcher(10);
}

void UMainWidget::OnClickedCustomTicketSaveButton()
{
	TicketCustomWidget->Btn_ResetBackground->SetVisibility(ESlateVisibility::Hidden);
	FinalTicketWidget->CaptureAndDisplayTicketBackground(TicketCustomWidget);
	SetWidgetSwitcher(8);
}

void UMainWidget::SelectConcertAnim(bool bIsRightBtn , int32 AnimNum)
{
	if (bIsRightBtn)
	{
		switch (AnimNum)
		{
		case 1:
			PlayAnimation(ConcertAnim1);
			break;
		case 2:
			PlayAnimation(ConcertAnim2);
			break;
		case 3:
			PlayAnimation(ConcertAnim3);
			break;
		case 4:
			PlayAnimation(ConcertAnim4);
			break;
		case 5:
			PlayAnimation(ConcertAnim5);
			break;
		default:
			break;
		}
	}

	else if (!bIsRightBtn)
	{
		switch (AnimNum)
		{
		case 1:
			PlayAnimation(ConcertAnim1_0);
			break;
		case 2:
			PlayAnimation(ConcertAnim2_1);
			break;
		case 3:
			PlayAnimation(ConcertAnim3_2);
			break;
		case 4:
			PlayAnimation(ConcertAnim4_3);
			break;
		case 5:
			PlayAnimation(ConcertAnim5_4);
			break;
		default:
			break;
		}
	}
}

void UMainWidget::SelectConcertInfoAnim(int32 InfoAnimNum)
{
	switch (InfoAnimNum)
	{
	case 1:
		PlayAnimation(ConcertInfoAnim01);
		break;
	case 2:
		PlayAnimation(ConcertInfoAnim02);
		break;
	case 3:
		PlayAnimation(ConcertInfoAnim03);
		break;
	case 4:
		PlayAnimation(ConcertInfoAnim04);
		break;
	case 5:
		PlayAnimation(ConcertInfoAnim05);
		break;
	default:
		break;
	}
}

void UMainWidget::SetInfoCanvas(int32 InfoAnimNum)
{
	switch (InfoAnimNum)
	{
	case 1:
		infoCanvas = Can_ConcertInfo01;
		break;
	case 2:
		infoCanvas = Can_ConcertInfo02;
		break;
	case 3:
		infoCanvas = Can_ConcertInfo03;
		break;
	case 4:
		infoCanvas = Can_ConcertInfo04;
		break;
	case 5:
		infoCanvas = Can_ConcertInfo05;
		break;

	default:
		break;
	}
}

void UMainWidget::SetInfoCanvasVisibility(UCanvasPanel* TargetCanvas , int32 InfoAnimNum)
{
	// TargetCanvas Visible, 나머지는 Hidden
	for (UCanvasPanel* Canvas : InfoCanvasPanels)
	{
		if (Canvas == TargetCanvas)
		{
			Canvas->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			Canvas->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void UMainWidget::OnClickedConcertL()
{
	ConcertNum++;
	//1~5 애니메이션 순환하기..
	if (ConcertNum > 5)
	{
		ConcertNum = 1;
		SelectConcertAnim(true , ConcertNum);
		SetInfoCanvas(ConcertNum);
		SelectConcertInfoAnim(ConcertNum);
		if (infoCanvas)
		{
			SetInfoCanvasVisibility(infoCanvas , ConcertNum);
		}
	}
	else
	{
		SetInfoCanvas(ConcertNum);
		SelectConcertInfoAnim(ConcertNum);
		SelectConcertAnim(true , ConcertNum);
		if (infoCanvas)
		{
			SetInfoCanvasVisibility(infoCanvas , ConcertNum);
		}
	}
}

void UMainWidget::OnClickedConcertR()
{
	//1~5 애니메이션 순환하기
	ConcertNum--;
	if (ConcertNum < 1)
	{
		ConcertNum = 5;
		SelectConcertAnim(false , ConcertNum);
		SetInfoCanvas(ConcertNum);
		SelectConcertInfoAnim(ConcertNum);
		if (infoCanvas)
		{
			SetInfoCanvasVisibility(infoCanvas , ConcertNum);
		}
	}
	else
	{
		SelectConcertAnim(false , ConcertNum);
		SetInfoCanvas(ConcertNum);
		SelectConcertInfoAnim(ConcertNum);
		if (infoCanvas)
		{
			SetInfoCanvasVisibility(infoCanvas , ConcertNum);
		}
	}
}

void UMainWidget::SetVisibleInteractionCan(bool visible)
{
	if (visible)
	{
		Can_Interaction->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Can_Interaction->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMainWidget::HandlePaymentPostpone()
{
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
	if (HttpActor2)
	{
		FString PostponeSeatId = FString::FromInt(HttpActor2->GetPostponeSeatId());
		HttpActor2->ReqPostGameResult(PostponeSeatId , GI->GetAccessToken());
	}
	SetWidgetSwitcher(1);
}

void UMainWidget::OnClickedTutorialStart()
{
	WS_Tutorial->SetActiveWidgetIndex(1);
}

void UMainWidget::OnClickedTutorialSkip()
{
	SetWidgetSwitcher(0);
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (!GI) return;
	GI->SetbIsNewPlayer(false);

	switch (GI->GetPlaceState())
	{
	case EPlaceState::Plaza:
		PlayTitleAnim(1);
		break;
	case EPlaceState::ConcertHall:
		PlayTitleAnim(2);
		break;
	case EPlaceState::CommunityHall:
		PlayTitleAnim(3);
		break;
	case EPlaceState::StyleLounge:
		PlayTitleAnim(4);
		break;
	default:
		break;
	}
}

void UMainWidget::OnClickedRight1()
{
	WS_Tutorial->SetActiveWidgetIndex(2);
}

void UMainWidget::OnClickedLeft2()
{
	WS_Tutorial->SetActiveWidgetIndex(1);
}

void UMainWidget::OnClickedRight2()
{
	WS_Tutorial->SetActiveWidgetIndex(3);
}

void UMainWidget::OnClickedLeft3()
{
	WS_Tutorial->SetActiveWidgetIndex(2);
}

void UMainWidget::OnClickedRight3()
{
	WS_Tutorial->SetActiveWidgetIndex(4);
}

void UMainWidget::OnClickedLeft4()
{
	WS_Tutorial->SetActiveWidgetIndex(3);
}

void UMainWidget::OnClickedRight4()
{
	WS_Tutorial->SetActiveWidgetIndex(5);
}

void UMainWidget::OnClickedLeft5()
{
	WS_Tutorial->SetActiveWidgetIndex(4);
}

void UMainWidget::OnClickedTutorialEnd()
{
	SetWidgetSwitcher(0);
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (!GI) return;
	GI->SetbIsNewPlayer(false);

	switch (GI->GetPlaceState())
	{
	case EPlaceState::Plaza:
		PlayTitleAnim(1);
		break;
	case EPlaceState::ConcertHall:
		PlayTitleAnim(2);
		break;
	case EPlaceState::CommunityHall:
		PlayTitleAnim(3);
		break;
	case EPlaceState::StyleLounge:
		PlayTitleAnim(4);
		break;
	default:
		break;
	}
}

void UMainWidget::HideAllTitle()
{
	Img_TitlePlaza->SetVisibility(ESlateVisibility::Hidden);
	Img_TitleConcertHall->SetVisibility(ESlateVisibility::Hidden);
	Img_TitleCommunityHall->SetVisibility(ESlateVisibility::Hidden);
	Img_TitleStyleLounge->SetVisibility(ESlateVisibility::Hidden);
}

void UMainWidget::PlayTitleAnim(int32 TitleNum)
{
	HideAllTitle();

	StopAnimation(TitlePlazaAnim);
	StopAnimation(TitleConcertHallAnim);
	StopAnimation(TitleCommunityHallAnim);
	StopAnimation(TitleStyleLoungeAnim);
	
	switch (TitleNum)
	{
	case 1:
		PlayAnimation(TitlePlazaAnim);
		break;
	case 2:
		PlayAnimation(TitleConcertHallAnim);
		break;
	case 3:
		PlayAnimation(TitleCommunityHallAnim);
		break;
	case 4:
		PlayAnimation(TitleStyleLoungeAnim);
		break;
	default:
		break;
	}
}
