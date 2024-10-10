// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTPlayerState.h"

void ATTPlayerState::BeginPlay()
{
	Super::BeginPlay();
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

int32 ATTPlayerState::GetUserId() const
{
	if ( !UserId )
	{
		UE_LOG(LogTemp , Warning , TEXT("UserId is empty , returning default value."));
		return -1;
	}
	return UserId;
}

void ATTPlayerState::SetCoin(const int32& _Coin)
{
	Coin = _Coin;
}

void ATTPlayerState::AddCoin(int32 _Coin)
{
	Coin += _Coin;
}

int32 ATTPlayerState::GetCoin()
{
	return Coin;
}

void ATTPlayerState::SetRemainingTicketCount(const int32& _RemainingTicketCount)
{
	RemainingTicketCount = _RemainingTicketCount;
}

void ATTPlayerState::UseRemainingTicket(int32 UsedTicketCount)
{
	RemainingTicketCount -= UsedTicketCount;
}

int32 ATTPlayerState::GetRemainingTicketCount()
{
	return RemainingTicketCount;
}

void ATTPlayerState::SetAvatarData(const int32& _AvatarData)
{
	AvatarData = _AvatarData;
}

int32 ATTPlayerState::GetAvatarData()
{
	return AvatarData;
}
