// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTLuckyDrawGameState.h"
#include "HJ/TTGameInstance.h"
//#include <HJ/TTPlayerState.h>
//#include "LHM/TTPlayerController.h"
#include <HJ/TTPlayer.h>
#include "EngineUtils.h"

void ATTLuckyDrawGameState::BeginPlay()
{
	Super::BeginPlay();

	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if ( GI ) GI->SetPlaceState(EPlaceState::LuckyDrawRoom);

	CurrentSeatNumber = 1;
}

void ATTLuckyDrawGameState::AssignSeatNumber(APlayerState* PlayerState)
{
    for ( TActorIterator<ATTPlayer> It(GetWorld()); It; ++It )
    {
        ATTPlayer* TTPlayer = *It;
        if ( TTPlayer && !TTPlayer->GetbIsHost() && TTPlayer->GetRandomSeatNumber() == -1 )
        {
            TTPlayer->SetRandomSeatNumber(CurrentSeatNumber++);

            // 디버그 메시지
            GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Green ,
                FString::Printf(TEXT("플레이어 %s에게 좌석 번호 %d가 할당되었습니다.") ,
                    *TTPlayer->GetNickname() , TTPlayer->GetRandomSeatNumber()));
        }

        TTPlayer->SwitchCamera(false);
    }
}