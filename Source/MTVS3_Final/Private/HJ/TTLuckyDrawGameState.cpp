// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTLuckyDrawGameState.h"
#include "HJ/TTGameInstance.h"
#include <HJ/TTPlayerState.h>
#include "LHM/TTPlayerController.h"
#include <HJ/TTPlayer.h>

void ATTLuckyDrawGameState::BeginPlay()
{
	Super::BeginPlay();

	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if ( GI ) GI->SetPlaceState(EPlaceState::LuckyDrawRoom);
}

void ATTLuckyDrawGameState::AssignSeatNumber(APlayerState* PlayerState)
{
	// PlayerState를 ATTPlayerState로 캐스팅
	ATTPlayerState* TTPS = Cast<ATTPlayerState>(PlayerState);
	if ( TTPS )
	{
		// PlayerController를 통해 Pawn에 접근
		ATTPlayerController* TTPC = Cast<ATTPlayerController>(TTPS->GetOwner());
		if ( TTPC )
		{
			// PlayerController의 Pawn을 ATTPlayer로 캐스팅
			ATTPlayer* TTPlayer = Cast<ATTPlayer>(TTPC->GetPawn());
			if ( TTPlayer && !TTPlayer->GetbIsHost() )
			{
				// 좌석 번호 할당 로직
				static int32 CurrentSeatNumber = 1;
				TTPlayer->SetRandomSeatNumber(CurrentSeatNumber++);

				// 디버그 메시지
				GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Green ,
					FString::Printf(TEXT("플레이어 %s에게 좌석 번호 %d가 할당되었습니다.") ,
						*TTPlayer->GetNickname() , TTPlayer->GetRandomSeatNumber()));
			}
		}
	}
}