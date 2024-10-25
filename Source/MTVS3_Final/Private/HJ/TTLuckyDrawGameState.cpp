// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTLuckyDrawGameState.h"
#include "HJ/TTGameInstance.h"
#include <HJ/TTPlayerState.h>

void ATTLuckyDrawGameState::BeginPlay()
{
	Super::BeginPlay();

	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if ( GI ) GI->SetPlaceState(EPlaceState::LuckyDrawRoom);
}

void ATTLuckyDrawGameState::AssignSeatNumber(APlayerState* PlayerState)
{
    ATTPlayerState* TTPlayerState = Cast<ATTPlayerState>(PlayerState);
    if ( TTPlayerState && !TTPlayerState->GetbIsHost() )
    {
        // 좌석 번호 할당 로직
        static int32 CurrentSeatNumber = 1;
        TTPlayerState->SetRandomSeatNumber(CurrentSeatNumber++);

        // 디버그 메시지
        GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Green ,
            FString::Printf(TEXT("플레이어 %s에게 좌석 번호 %d가 할당되었습니다.") ,
                *TTPlayerState->GetNickname() , TTPlayerState->GetRandomSeatNumber()));
    }
}