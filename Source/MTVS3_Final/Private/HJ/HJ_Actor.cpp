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

			// 코인 더하기 및 가져오기
			// 더하기로 되어 있으므로 예매 시엔 음수(-) 값 입력
			PS->AddCoin(-30);
			int32 Coin = PS->GetCoin();

			// 티켓 접수 및 접수 가능 개수 가져오기
			// UseRemainingTicket의 매개변수는 티켓 접수 개수
			PS->UseRemainingTicket(1);
			int32 RemainingTicketCount = PS->GetRemainingTicketCount();
		}
	}
}

