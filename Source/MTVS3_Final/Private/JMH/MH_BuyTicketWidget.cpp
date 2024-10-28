// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_BuyTicketWidget.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor.h"
#include "HJ/TTGameInstance.h"
#include "LHM/HM_HttpActor2.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/EditableText.h"

class AHM_HttpActor;


void UMH_BuyTicketWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Confirm01->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm01Button);
	Btn_Confirm_QRFailed->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm01Button);
	//Btn_Back01->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBack01Button);
	Btn_Confirm02->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm02Button);
	Btn_Back02->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBack02Button);
	Btn_Confirm_QRUi1->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm_QRUi1Button);
	Btn_Back_QRUi1->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBack_QRUi1Button);
	Btn_Confirm_QRSuccess->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm_QRUi2SuccessButton);
	Btn_Back_QRFailed->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm_QRUiFailedButton);
	Btn_AddCoin->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedAddCoinButton);
	Btn_BuyTicketCoin->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBuyTicketCoinButton);
	Btn_Back03->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBack03Button);
	//Btn_SaveTicket->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::티켓저장버튼);
	Btn_Back04->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBtn_Back04Button);
	Btn_Back05->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBtn_Back05Button);

	if(Btn_BuyTickerBack)
	{
		Btn_BuyTickerBack->OnClicked.AddDynamic(this,&UMH_BuyTicketWidget::CloseButtonPressed);
	}
}

void UMH_BuyTicketWidget::SetWidgetSwitcher(int32 num)//0:티켓예매정보,1:QR,2:QR성공,3:QR실패,4:배송지,5:결제진행,7:코인충전
{
	//서버에서 불러와서 입력
	WS_BuyTicketSwitcher->SetActiveWidgetIndex(num);
}

void UMH_BuyTicketWidget::SetTextTicketPrice(int32 TicketPrice)
{
	Text_TicketPrice1->SetText(FText::AsNumber(TicketPrice));
	Text_TicketPrice1_1->SetText(FText::AsNumber(TicketPrice));
	Text_TicketPrice2->SetText(FText::AsNumber(TicketPrice));
	Text_TicketPrice2_2->SetText(FText::AsNumber(TicketPrice));
	Text_TicketPrice3->SetText(FText::AsNumber(TicketPrice));
}

// HTTP : 회원 인증 QR 요청
void UMH_BuyTicketWidget::OnClickedConfirm01Button()
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

// 결제시 서버에서 받아온 회원 인증 QR 이미지 넣어주기
void UMH_BuyTicketWidget::SetQRImg(UTexture2D* newTexture)
{
	Img_QR->SetBrushFromTexture(newTexture);
}

// HTTP : 예매자 정보 입력 요청
void UMH_BuyTicketWidget::OnClickedConfirm02Button()
{
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
	UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		if ( HttpActor2 )
		{
			HttpActor2->ReqPostReservationinfo(EText_Name->GetText() , EText_PhoneNum->GetText(), EText_Address1->GetText(), EText_Address2->GetText(), gi->GetAccessToken());
		}
	}
}

void UMH_BuyTicketWidget::OnClickedBack02Button()
{
	//배송지 입력에서 취소버튼
	//다시 
	//0 : 티켓 예매정보 확인으로
	SetWidgetSwitcher(0);

}

// HTTP : 결제시 회원 인증 사진 업로드 확인
void UMH_BuyTicketWidget::OnClickedConfirm_QRUi1Button()
{
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		if ( HttpActor2 )
		{
			HttpActor2->ReqGetPostConfirmMemberPhoto(HttpActor2->GetUserCode() , gi->GetAccessToken());
			UE_LOG(LogTemp , Log , TEXT("HttpActor2->GetUserCode() : %s"), *HttpActor2->GetUserCode());
		}
	}
}

void UMH_BuyTicketWidget::OnClickedBack_QRUi1Button()
{	//QR UI
	//1 : 예매정보 확인창으로
	SetWidgetSwitcher(0);
}

void UMH_BuyTicketWidget::OnClickedConfirm_QRUi2SuccessButton()
{
	//QR 인증 성공시 확인버튼
	//세션이동 희진 -> 성공실패 정보 들고와서 -> 성공시 배송지 입력창으로
	//일단
	//4: 배송지 입력창으로
	SetWidgetSwitcher(4);
}

void UMH_BuyTicketWidget::OnClickedConfirm_QRUiFailedButton()
{
	//QR실패시 다시 QR
	//1: QRUI
	SetWidgetSwitcher(1);
}

void UMH_BuyTicketWidget::SetTextSeatID(FString SeatID)
{
	//서버에서? 유저가 접수한 좌석정보
	/*AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
	if ( HttpActor2 )
	{
		HttpActor2->GetMyReceptionSeats();
	}*/
}

void UMH_BuyTicketWidget::SetTextTotalCoin(int32 TotalCoin)
{
	Text_TotalCoin->SetText(FText::AsNumber(TotalCoin));
	Text_TotalCoin_1->SetText(FText::AsNumber(TotalCoin));
}

void UMH_BuyTicketWidget::SetTextNeedCoin(int32 NeedCoin)
{
	Text_NeedCoin->SetText(FText::AsNumber(NeedCoin));
}

void UMH_BuyTicketWidget::SetTextTicketNum(int32 TicketNum)
{
	Text_TicketNum->SetText(FText::AsNumber(TicketNum));
	Text_TicketNum2->SetText(FText::AsNumber(TicketNum));
	Text_TicketNum3->SetText(FText::AsNumber(TicketNum));
	Text_TicketNum4->SetText(FText::AsNumber(TicketNum));
}

void UMH_BuyTicketWidget::OnClickedAddCoinButton()
{
	//결제 진행 위젯에서
	//충전 버튼 누르면
	//7 : 충전 위젯으로
	SetWidgetSwitcher(7);
	
}

void UMH_BuyTicketWidget::OnClickedBack03Button()
{
	//결제 진행 
	//뒤로가기 누르면 다시 배송지로
	//4: 배송지 입력으로 이동
	SetWidgetSwitcher(4);
}

// HTTP : 좌석 결제 요청
void UMH_BuyTicketWidget::OnClickedBuyTicketCoinButton()
{
	//코인 결제
	//유저 코인이 결제 가능한 정도라면 결제완료창으로 //현민
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		AHM_HttpActor2* HttpActor2 = Cast<AHM_HttpActor2>(
			UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor2::StaticClass()));
		if ( HttpActor2 )
		{
			HttpActor2->ReqPostPaymentSeat(gi->GetConcertName(), HttpActor2->GetMySeatId() , gi->GetAccessToken());
			UE_LOG(LogTemp , Log , TEXT("HttpActor2->GetMySeatId(): %s") , *HttpActor2->GetMySeatId());
		}
	}
	//아니라면
	//충전해야한다고 경고창
}

void UMH_BuyTicketWidget::SetTextSeatID2(FString SeatID2)
{
	Text_SeatID2->SetText(FText::FromString(SeatID2));
}

void UMH_BuyTicketWidget::SetTextUserName(FString UserName)
{
	Tex_UserName->SetText(FText::FromString(UserName));
}

void UMH_BuyTicketWidget::SetTextUserPhoneNum(FString UserPhoneNum)
{
	Tex_UserPhoneNum->SetText(FText::FromString(UserPhoneNum));
}

void UMH_BuyTicketWidget::SetTextUserAddress(FString UserAddress)
{
	Tex_Address->SetText(FText::FromString(UserAddress));
}

void UMH_BuyTicketWidget::OnClickedSaveTicketButton()
{
	//서버에 티켓정보 저장
	// 서버에 저장 현민
	// 저장되었다는알림
}

void UMH_BuyTicketWidget::OnClickedBtn_Back04Button()
{
	//결제가 취소되었습니다 알림
	//위젯종료
}

void UMH_BuyTicketWidget::OnClickedBtn_Back05Button()
{
	//티켓 결제에서 
	//코인 결제창에서 뒤로가기 누르면
	//5 :결제진행창으로
	SetWidgetSwitcher(5);
}
