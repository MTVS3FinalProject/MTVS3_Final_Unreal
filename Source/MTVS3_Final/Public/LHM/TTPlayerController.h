// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "TTPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API ATTPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	float TimeReqInterval; // 요청 간격
	float TimeSinceLastReq; // 마지막 요청 이후 경과 시간
    FDateTime DrawStartTime; // 추첨 시작 시간을 저장하는 변수

public:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

#pragma region UI
	void InitMainUI();

	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UMainWidget> MainUIFactory;
	UPROPERTY()
	class UMainWidget* MainUI;

	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UMH_TicketingWidget> TicketingUIFactory;
	UPROPERTY()
	class UMH_TicketingWidget* TicketingUI;

	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UMH_WorldMap> WorldMapUIFactory;
	UPROPERTY()
	class UMH_WorldMap* WorldMapUI;

	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UMH_GameWidget> GameUIFactory;
	UPROPERTY()
	class UMH_GameWidget* GameUI;
	
	void InitGameUI();
	void SetTextMyNum();

	UFUNCTION(Client , Reliable)
	void ClientLuckyDrawLose();

	UFUNCTION(Client , Reliable)
	void ClientLuckyDrawWin();
	
	// UPROPERTY()
	// class UMH_TicketingWidget* TicketingUI;
 //    void SetTicketingUI(UMH_TicketingWidget* InTicketingUI);
 //
 //    UPROPERTY()
 //    class UMainWidget* MainUI;
 //    void SetMainUI(UMainWidget* InMainUI);
#pragma endregion

	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Input")
	class UInputMappingContext* IMC_TTPlayer;
	
    // 서버와 클라이언트의 시간 동기화
    UFUNCTION(Server, Reliable)
    void ServerGetCurrentTime();

    UFUNCTION(Client, Reliable)
    void ClientReceiveCurrentTime(const FString& CurrentTime);

    // 현재 시스템 시간 가져오기
    FString GetSystemTime();

    // 추첨 시작 시간 설정
    void SetDrawStartTime();

    // 카운트다운 업데이트
    void UpdateCountdown(float DeltaTime);
    
    // 서버 시간 요청
    void RequestServerTime();
};
