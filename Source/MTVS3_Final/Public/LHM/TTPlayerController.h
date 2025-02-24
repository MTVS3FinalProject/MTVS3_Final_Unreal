﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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

	UPROPERTY()
	class UMH_TicketingWidget* TicketingUI;

    void SetTicketingUI(UMH_TicketingWidget* InTicketingUI);

    UPROPERTY()
    class UMainWidget* MainUI;

    void SetMainUI(UMainWidget* InMainUI);
#pragma endregion

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

	UFUNCTION(Server, Reliable)
	void ServerUpdateNewSeats(const FString& SeatId);

	//MH
	
	//메시지 클라이언트들에게 전송하는 서버 함수
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSendChatMessage(const FString& ServerMessage);
	
	// ChatManager에 직접 접근하는 함수
	void SetChatManager(class AMH_ChatManager* InChatManager);

	UFUNCTION(BlueprintCallable, Category = "Chat")
	void SendChatMessage(const FString& Message);

	UPROPERTY()
	class AMH_ChatManager* ChatManagerInstance;

	UFUNCTION()
	void OnChatMessageReceived(const FString& Message);

	FTimerHandle CountdownTimerHandle;

	void EndPlay(const EEndPlayReason::Type EndPlayReason);

	// LHM_Tree
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerRequestApplyTicketImage(int32 TicketIndex, const FString& TicketImgUrl);
};
