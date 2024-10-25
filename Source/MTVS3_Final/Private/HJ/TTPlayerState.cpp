// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTPlayerState.h"
#include "HJ/TTGameInstance.h"
#include "Net/UnrealNetwork.h"

void ATTPlayerState::BeginPlay()
{
	Super::BeginPlay();

	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (GI) SetNickname(GI->GetNickname());
}

void ATTPlayerState::SetNickname(const FString& _Nickname)
{
	Nickname = _Nickname;
};

void ATTPlayerState::SetLuckyDrawSeatID(const FString& _LuckyDrawSeatID)
{
	LuckyDrawSeatID = _LuckyDrawSeatID;
	
	// GI에도 SetLuckyDrawSeatID
	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (GI) GI->SetLuckyDrawSeatID(LuckyDrawSeatID);
}

void ATTPlayerState::SetRandomSeatNumber(const int32& _RandomSeatNumber)
{
	RandomSeatNumber = _RandomSeatNumber;
}
