// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/TTPlayerController.h"
#include "Engine/World.h"
#include "chrono"
#include "JMH/MH_TicketingWidget.h"
#include "Kismet/GameplayStatics.h"
#include "JMH/MainWidget.h"

void ATTPlayerController::BeginPlay()
{
    Super::BeginPlay();

	TicketingUI = CastChecked<UMH_TicketingWidget>(CreateWidget(GetWorld() , TicketingUIFactory));
	if ( TicketingUI )
	{
		TicketingUI->AddToViewport();
		TicketingUI->SetWidgetSwitcher(0);
        //TicketingUI->SetVisibleSwitcher(true); // 테스트용
	}

    MainUI = CastChecked<UMainWidget>(CreateWidget(GetWorld(), MainUIFactory));
    
    // 테스트용
    /*if ( MainUI )
    {
        MainUI->AddToViewport();
    }

    auto* pc = UGameplayStatics::GetPlayerController(this , 0);
    if ( !pc ) return;
    pc->SetShowMouseCursor(true);
    pc->SetInputMode(FInputModeGameAndUI());*/

	// 추첨 시작 시간 설정
    SetDrawStartTime();
}

void ATTPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

	TimeReqInterval = 0.5f;
	TimeSinceLastReq += DeltaTime;

	if( TimeSinceLastReq >= TimeReqInterval )
    {
        RequestServerTime();
        TimeSinceLastReq = 0.0f; // 요청 후 리셋
    }

    // 카운트다운 업데이트
    UpdateCountdown(DeltaTime);
}

void ATTPlayerController::RequestServerTime()
{
    if ( HasAuthority() )
    {
        // 서버에서 현재 시간 요청
        ServerGetCurrentTime();
    }
}

void ATTPlayerController::ServerGetCurrentTime_Implementation()
{
    // 현재 시스템 시간 가져오기
    FString CurrentTime = GetSystemTime();
    ClientReceiveCurrentTime(CurrentTime);
}

void ATTPlayerController::ClientReceiveCurrentTime_Implementation(const FString& CurrentTime)
{
    // HUD에 현재 시간 표시 (HUD 업데이트 함수 호출)
    MainUI->SetTextCurrentTime(CurrentTime);
    UE_LOG(LogTemp , Log , TEXT("Current Time from Server: %s") , *CurrentTime);
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
    std::strftime(buffer , sizeof(buffer) , "%Y-%m-%d %H:%M:%S" , &localTime);

    // FString로 변환하여 Unreal에서 출력
    return FString(buffer);
}

void ATTPlayerController::SetDrawStartTime()
{
    // 현재 시스템 시간 가져오기
    FDateTime Now = FDateTime::Now();

    // 시연용
    // 현재 날짜, 임의로 설정한 추첨 시작 시간
    DrawStartTime = FDateTime(Now.GetYear() , Now.GetMonth() , Now.GetDay() , 11 , 0 , 0);

    // DrawStartTime을 hour:minute 형식으로 변환
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
