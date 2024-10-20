﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/HJ_Actor.h"
#include "HJ/TTPlayer.h"
#include "HJ/TTGameInstance.h"

// Sets default values
AHJ_Actor::AHJ_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHJ_Actor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHJ_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if ( GI )
	{
		// 플레이어 데이터를 구조체로 가져오기
		//FPlayerData PlayerData = GI->GetPlayerData();  // 현재 플레이어 데이터를 가져옴
		//// 관리자 여부 설정 및 가져오기
		//PlayerData.bIsHost = true;
		//bool bIsHost = PlayerData.bIsHost;
		//GI->SetPlayerData(PlayerData);
		GI->SetbIsHost(true);
		bool bIsHost = GI->GetbIsHost();

		// 닉네임 설정 및 가져오기
		GI->SetNickname("Nickname");
		FString Nickname = GI->GetNickname();

		// 코인 서버에서 받아오기, 더하기, 가져오기
		// 더하기로 되어 있으므로 예매 시엔 음수(-) 값 입력
		GI->SetCoin(/*코인 저장 변수*/100000);
		GI->AddCoin(-30);
		int32 Coin = GI->GetCoin();

		// 티켓 접수 가능 개수 서버에서 받아오기, 티켓 접수, 접수 가능 개수 가져오기
		// UseRemainingTicket의 매개변수는 티켓 접수 개수
		GI->SetRemainingTicketCount(/*티켓 접수 가능 개수 저장 변수*/2);
		GI->UseRemainingTicket(1);
		int32 RemainingTicketCount = GI->GetRemainingTicketCount();

		// 아바타 데이터 서버에서 받아오기, 아바터 데이터 가져오기
		GI->SetAvatarData(/*아바타 데이터 저장 변수*/1);
		int32 AvatarData = GI->GetAvatarData();
	}
}

