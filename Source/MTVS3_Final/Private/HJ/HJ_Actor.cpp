// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/HJ_Actor.h"
#include "HJ/TTPlayer.h"
#include "HJ/TTPlayerState.h"

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

	ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if ( TTPlayer )
	{
		if ( ATTPlayerState* PS = TTPlayer->GetPlayerState<ATTPlayerState>())
		{
			// 관리자 여부 설정 및 가져오기
			PS->SetbIsHost(true);
			bool bIsHost = PS->GetbIsHost();

			// 닉네임 설정 및 가져오기
			PS->SetNickname("Nickname");
			FString Nickname = PS->GetNickname();

			// 서버에서 주는 UserId 설정 및 가져오기
			// 로그인 시 HTTP 통신으로 응답을 받아와 저장하는 방식
			PS->SetUserId(12345);
			int32 UserId = PS->GetUserId();

			// 나이 서버에서 받아오기, 나이 가져오기
			PS->SetAge(/*나이 저장 변수*/20);
			int32 Age = PS->GetAge();

			// 코인 서버에서 받아오기, 더하기, 가져오기
			// 더하기로 되어 있으므로 예매 시엔 음수(-) 값 입력
			PS->SetCoin(/*코인 저장 변수*/100000);
			PS->AddCoin(-30);
			int32 Coin = PS->GetCoin();

			// 티켓 접수 가능 개수 서버에서 받아오기, 티켓 접수, 접수 가능 개수 가져오기
			// UseRemainingTicket의 매개변수는 티켓 접수 개수
			PS->SetRemainingTicketCount(/*티켓 접수 가능 개수 저장 변수*/2);
			PS->UseRemainingTicket(1);
			int32 RemainingTicketCount = PS->GetRemainingTicketCount();

			// 아바타 데이터 서버에서 받아오기, 아바터 데이터 가져오기
			PS->SetAvatarData(/*아바타 데이터 저장 변수*/1);
			int32 AvatarData = PS->GetAvatarData();
		}
	}
}

