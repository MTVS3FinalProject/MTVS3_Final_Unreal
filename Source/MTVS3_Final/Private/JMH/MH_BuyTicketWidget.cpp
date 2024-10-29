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
	Btn_Confirm02->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm02Button);
	Btn_Confirm_QRUi1->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm_QRUi1Button);
	Btn_Confirm_QRSuccess->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm_QRUi2SuccessButton);
	Btn_Back_QRFailed->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm_QRUiFailedButton);
	Btn_AddCoin->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedAddCoinButton);
	Btn_BuyTicketCoin->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBuyTicketCoinButton);
	Btn_Back03->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBack03Button);
	Btn_Back04->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBtn_Back04Button);
	Btn_Back05->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBtn_Back05Button);
	Btn_BuyTicket2->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBuyTicket2Button);
	Btn_Confirm02_1->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm02_1Button);
	Btn_Confirm01_1->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm01_1Button);
	Btn_CompleteBuyTicket->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedCompleteBuyTicketButton);
	Btn_Confirm_QRFailed->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm_QRUiFailedButton);
	//Btn_Back01->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBack01Button);
	//Btn_Back02->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBack02Button);
	//Btn_Back_QRUi1->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBack_QRUi1Button);
	//Btn_SaveTicket->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::티켓저장버튼);

	if(Btn_BuyTickerBack)
	{
		Btn_BuyTickerBack->OnClicked.AddDynamic(this,&UMH_BuyTicketWidget::CloseButtonPressed);
	}
	auto* gi = Cast<UTTGameInstance>(GetWorld()->GetGameInstance());
	if ( gi )
	{
		Text_ConcertName_001->SetText(FText::FromString(gi->GetConcertName()));
	}
}

void UMH_BuyTicketWidget::SetWidgetSwitcher(int32 num)//0:티켓예매정보,1:QR,2:QR성공,3:QR실패,4:배송지,5:결제진행,7:코인충전
{
	//서버에서 불러와서 입력
	WS_BuyTicketSwitcher->SetActiveWidgetIndex(num);
}

void UMH_BuyTicketWidget::SetTextTicketPrice(int32 TicketPrice)
{
	Text_TicketPrice01->SetText(FText::AsNumber(TicketPrice));
	//Text_TicketPrice1_1->SetText(FText::AsNumber(TicketPrice));
	Text_TicketPrice2->SetText(FText::AsNumber(TicketPrice));
	Text_TicketPrice2_2->SetText(FText::AsNumber(TicketPrice));
	//Text_TicketPrice3->SetText(FText::AsNumber(TicketPrice));
}

//3: 좌석 확인 계속하기 버튼
void UMH_BuyTicketWidget::OnClickedConfirm01Button()
{
	//4 : 티켓 가격 확인으로
	SetWidgetSwitcher(4);
}

//4: 티켓 가격 확인 
void UMH_BuyTicketWidget::OnClickedConfirm01_1Button()
{
	//5 : 예매자 정보 입력으로
	SetWidgetSwitcher(5);
}
//6: 배송지 입력
void UMH_BuyTicketWidget::OnClickedConfirm02_1Button()
{
	//현민 : 주소 서버에 전달(예매자 정보랑 같이 한번에 넘길지? 따로 넘길지?)
	//7 : 결제수단 선택 창
	SetWidgetSwitcher(7);
}

void UMH_BuyTicketWidget::OnClickedRecentAddress()
{
	//현민
	//최근 배송지 불러와서 셋해주기?
	
}

// HTTP : 회원 인증 QR 요청(메인으로 이사가야함)
//현민 : 원래 티켓 예매 정보 확인 후 QR로 넘어가는 확인버튼. 근데 이제 메인 _ Can_Game_Win에서 예매 진행하기 버튼 누르면 BuyTicket->SetQRImg()해줘야함..
/*
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
}*/

// 결제시 서버에서 받아온 회원 인증 QR 이미지 넣어주기
void UMH_BuyTicketWidget::SetQRImg(UTexture2D* newTexture)
{
	Img_QR->SetBrushFromTexture(newTexture);
}

//5: 예매자 정보 입력
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

	//현민 : 여긴 수정안해도 될거같은데 일단 체크\
	//(예매자 정보랑 같이 한번에 넘길지? 따로 넘길지?)
	//계속 진행하기 버튼 누르면 배송지 입력으로 가야함.
	//6 : 배송지 입력으로 SetWidgetSwitcher(6);
}

//7: 결제 수단 결제하기 버튼
void UMH_BuyTicketWidget::OnClickedBuyTicket2Button()
{
	//8: 결제완료 창으로
	SetWidgetSwitcher(8);
}

//8:결제 완료
void UMH_BuyTicketWidget::OnClickedCompleteBuyTicketButton()
{
	//9: 예매 완료 
	//매희 : 메인에서 델리게이트 바인드, 메인 UI 위젯스위처 0번으로
	
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
	//현민 : 인증 완료버튼 -> 성공 : SetWidgetSwitcher(1); /실패 : SetWidgetSwitcher(2);이동
}
/*
void UMH_BuyTicketWidget::OnClickedBack_QRUi1Button()
{	//QR UI
	//매희 -> MainUI랑 델리게이트로 연결 0번 위젯으로 옮겨주기
	//1 : 예매정보 확인창으로
	SetWidgetSwitcher(0);
}*/

void UMH_BuyTicketWidget::OnClickedConfirm_QRUi2SuccessButton()
{
	//QR 인증 성공시 확인버튼
	//세션이동 희진 -> 성공실패 정보 들고와서 -> 성공시 배송지 입력창으로
	//일단
	//(수정전)4: 배송지 입력창으로
	//(수정후)3: 좌석 확인창으로
	SetWidgetSwitcher(3);
}

void UMH_BuyTicketWidget::OnClickedConfirm_QRUiFailedButton()
{
	//QR실패시 다시 QR
	//현민 : 여기서 다시 QR 생성? 아님 한번 생성된 QR은 그자리에 있나?
	//1: QRUI
	SetWidgetSwitcher(0);
}

void UMH_BuyTicketWidget::OnClickedBack_QRFailedButton()
{

	//매희 :Main 이랑 델리게이트 0번 스위처로 옮겨주기
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
	//Text_TicketNum3->SetText(FText::AsNumber(TicketNum));
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
	//Text_SeatID->SetText(FText::FromString(SeatID2));
}

void UMH_BuyTicketWidget::SetTextUserName(FString UserName)
{
	Text_UserName->SetText(FText::FromString(UserName));
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
