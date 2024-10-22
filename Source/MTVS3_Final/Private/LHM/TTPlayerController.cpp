// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/TTPlayerController.h"
#include "Engine/World.h"
#include "chrono"
#include "Kismet/GameplayStatics.h"
#include "JMH/MH_TicketingWidget.h"
#include "JMH/MainWidget.h"

void ATTPlayerController::BeginPlay()
{
    Super::BeginPlay();

    
	/*TicketingUI = CastChecked<UMH_TicketingWidget>(CreateWidget(GetWorld() , TicketingUIFactory));
	if ( TicketingUI )
	{
		TicketingUI->AddToViewport();
		TicketingUI->SetWidgetSwitcher(0);
		TicketingUI->SetVisibleSwitcher(true);
	}

    auto* pc = UGameplayStatics::GetPlayerController(this , 0);
    if ( !pc ) return;
    pc->SetShowMouseCursor(true);
    pc->SetInputMode(FInputModeGameAndUI());*/

	// 추첨 시작 시간 설정
    //SetDrawStartTime();
}

void ATTPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeReqInterval = 0.5f;
	TimeSinceLastReq += DeltaTime;
	if ( TimeSinceLastReq >= TimeReqInterval )
	{
		RequestServerTime();
		TimeSinceLastReq = 0.0f;
	}

	// 카운트다운 업데이트
	UpdateCountdown(DeltaTime);
}

void ATTPlayerController::RequestServerTime()
{
	// 서버에서 현재 시간 요청
	ServerGetCurrentTime();
}  

void ATTPlayerController::ServerGetCurrentTime_Implementation()
{
    // 현재 시스템 시간 가져오기
    FString CurrentTime = GetSystemTime();

	ClientReceiveCurrentTime(CurrentTime);
}

void ATTPlayerController::ClientReceiveCurrentTime_Implementation(const FString& CurrentTime)
{
    // Main UI에 현재 시간 표시
    /*if ( MainUI )
	{
		MainUI->SetTextCurrentTime(CurrentTime);
	}*/

    // 클라이언트의 로컬 UI만 업데이트
    if ( IsLocalController() ) // 로컬 플레이어의 컨트롤러인 경우에만
    {
        if ( MainUI )
        {
            MainUI->SetTextCurrentTime(CurrentTime);
        }
    }
    //UE_LOG(LogTemp , Log , TEXT("Current Time from Server: %s") , *CurrentTime);
}

void ATTPlayerController::SetTicketingUI(UMH_TicketingWidget* InTicketingUI)
{
    TicketingUI = InTicketingUI; // 전달받은 TicketingUI 참조 저장
}

void ATTPlayerController::SetMainUI(UMainWidget* InMainUI)
{
    MainUI = InMainUI;  // 전달받은 MainUI 참조 저장
}

FString ATTPlayerController::GetSystemTime()
{
	// 현재 시스템 시간을 가져오기
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    // tm 구조체로 변환 (로컬 시간)
    std::tm localTime;
    localtime_s(&localTime , &currentTime);

    // 시간 포맷 설정 (yyyy-MM-dd HH:mm:ss)
    char buffer[100];
    std::strftime(buffer , sizeof(buffer) , "%H:%M" , &localTime);
    //std::strftime(buffer , sizeof(buffer) , "%Y-%m-%d %H:%M:%S" , &localTime);

    // FString로 변환하여 Unreal에서 출력
    return FString(buffer);
}

void ATTPlayerController::SetDrawStartTime()
{
    // 현재 시스템 시간 가져오기
    FDateTime Now = FDateTime::Now();

    // 현재 시간으로부터 10분 후로 설정
    //DrawStartTime = Now + FTimespan(0 , 10 , 0); // 10분 후

    // 현재 날짜, 임의로 설정한 추첨 시작 시간
    DrawStartTime = FDateTime(Now.GetYear() , Now.GetMonth() , Now.GetDay() , 22 , 0 , 0);

    // DrawStartTime을 원하는 형식으로 변환
    int32 Hours = DrawStartTime.GetHour();
    int32 Minutes = DrawStartTime.GetMinute();

    // 포맷된 시간 문자열 생성
    FString FormattedTime = FString::Printf(TEXT("%02d:%02d") , Hours , Minutes);

    // UI에 텍스트 설정
    if ( TicketingUI )
    {
		TicketingUI->SetTextTicketingDeadline(FormattedTime);
    }
}

void ATTPlayerController::UpdateCountdown(float DeltaTime)
{
	// 현재 시스템 시간 가져오기
	FDateTime Now = FDateTime::Now();

	// 남은 시간 계산
	FTimespan RemainingTime = DrawStartTime - Now;

	if ( RemainingTime.GetTotalSeconds() > 0.5f )
	{
		int32 Minutes = RemainingTime.GetMinutes();
		int32 Seconds = RemainingTime.GetSeconds() % 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d") , Minutes , Seconds);

		if ( TicketingUI )
		{
			TicketingUI->SetTextGameStartTime(CountdownText);
		}
		//UE_LOG(LogTemp , Log , TEXT("%s") , *CountdownText);  // 로그로 출력
	}
}
