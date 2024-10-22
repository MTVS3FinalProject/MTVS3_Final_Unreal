// Fill out your copyright notice in the Description page of Project Settings.


#include "JMH/MH_BuyTicketWidget.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor.h"

class AHM_HttpActor;

void UMH_BuyTicketWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Btn_Confirm01->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm01Button);
	//Btn_Back01->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBack01Button);
	Btn_Confirm02->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm02Button);
	Btn_Back02->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBack02Button);
	Btn_Confirm_QRUi1->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm_QRUi1Button);
	Btn_Back_QRUi1->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBack_QRUi1Button);
	Btn_Confirm_QRSuccess->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm_QRUi2SuccessButton);
	Btn_Confirm_QRFailed->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedConfirm_QRUiFailedButton);
	Btn_AddCoin->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedAddCoinButton);
	Btn_BuyTicketCoin->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedAddCoinButton);
	Btn_Back03->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBack03Button);
	Btn_SaveTicket->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBuyTicketCoinButton);
	Btn_Back04->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBtn_Back04Button);
	Btn_Back05->OnClicked.AddDynamic(this , &UMH_BuyTicketWidget::OnClickedBtn_Back05Button);
}

void UMH_BuyTicketWidget::SetWidgetSwitcher(int32 num)//0:티켓예매정보,1:QR,2:QR성공,3:QR실패,4:배송지,5:결제진행,7:코인충전
{
	
	//서버에서 불러와서 입력
	WS_BuyTicketSwitcher->SetActiveWidgetIndex(num);
}

void UMH_BuyTicketWidget::OnClickedConfirm01Button()
{
	SetWidgetSwitcher(1);
}

//void UMH_BuyTicketWidget::OnClickedBack01Button()
//{
	//뒤로가기 하면 어디로?// 비지블 끄기, 위젯 스위쳐 옮가기?
	
	
	//메인에선 닫기 해야해서 그냥 이거 없애고 거기서 버튼 붙여주기
//}

//배송지 입력에서 확인버튼
void UMH_BuyTicketWidget::OnClickedConfirm02Button()
{
	//이름, 번호, 주소 서버로 보내기 현민
	//제대로 입력되었고 성공하면
	//5:결제 진행으로 SetWidgetSwitcher(5);
}

void UMH_BuyTicketWidget::OnClickedBack02Button()
{
	//배송지 입력에서 취소버튼
	//다시 
	//0 : 티켓 예매정보 확인으로
	SetWidgetSwitcher(0);

}

void UMH_BuyTicketWidget::OnClickedConfirm_QRUi1Button()
{
	//QR 인증 확인 요청
	AHM_HttpActor* HttpActor = Cast<AHM_HttpActor>(
		UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor::StaticClass()));
	if (HttpActor)
	{
		//결제창에서 이메일을 서버에 보내주려면... 게임인스턴스에 유저이메일 저장해줘야하나?
		//HttpActor->ReqPostVerifyIdentity(EText_Email->GetText());
	}
	else
	{
		//에러창
		GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Red , TEXT("ClickedSignUp Error"));
	}
	//서버에서 성공시 SetWidgetSwitcher(2);로 이동해주세여 현민~
	//서버에서 실패시 SetWidgetSwitcher(3);로 이동해주세여 현민~
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
}

void UMH_BuyTicketWidget::SetTextTicketNum(FString TicketNum)
{
	
}

void UMH_BuyTicketWidget::SetTextTicketPrice(int32 TicketPrice)
{
	//
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

void UMH_BuyTicketWidget::OnClickedBuyTicketCoinButton()
{
	//코인 결제
	//유저 코인이 결제 가능한 정도라면 결제완료창으로 //현민
	//SetWidgetSwitcher(6);
	//아니라면
	//충전해야한다고 경고창
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
