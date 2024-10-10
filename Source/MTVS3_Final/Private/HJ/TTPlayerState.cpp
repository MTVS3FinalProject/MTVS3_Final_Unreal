// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTPlayerState.h"

void ATTPlayerState::BeginPlay()
{
	Super::BeginPlay();

	Coin = InitialCoin;
	RemainingTicketCount = MaxRemainingTicketCount;
}

void ATTPlayerState::SetbIsHost(const bool& _bIsHost)
{
	bIsHost = _bIsHost;
}

bool ATTPlayerState::GetbIsHost() const
{
	return bIsHost;
}

void ATTPlayerState::SetNickname(const FString& _Nickname)
{
	Nickname = _Nickname;
}

FString ATTPlayerState::GetNickname() const
{
	if ( Nickname.IsEmpty() )
	{
		UE_LOG(LogTemp , Warning , TEXT("Nickname is empty , returning default value."));
		return TEXT("Empty_Nickname");
	}
	return Nickname;
}

void ATTPlayerState::SetUserId(const int32& _UserId)
{
	UserId = _UserId;
}

long ATTPlayerState::GetUserId() const
{
	if ( !UserId )
	{
		UE_LOG(LogTemp , Warning , TEXT("UserId is empty , returning default value."));
		return -1;
	}
	return UserId;
}

void ATTPlayerState::AddCoin(int32 _Coin)
{
	Coin += _Coin;
}

int32 ATTPlayerState::GetCoin()
{
	return Coin;
}

void ATTPlayerState::UseRemainingTicket(int32 UsedTicketCount)
{
	RemainingTicketCount -= UsedTicketCount;
}

int32 ATTPlayerState::GetRemainingTicketCount()
{
	return RemainingTicketCount;
}
