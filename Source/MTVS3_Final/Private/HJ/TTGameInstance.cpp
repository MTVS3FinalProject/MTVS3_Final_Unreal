// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTGameInstance.h"

void UTTGameInstance::SetbIsHost(const bool& _bIsHost)
{
	bIsHost = _bIsHost;
}

bool UTTGameInstance::GetbIsHost() const
{
	return bIsHost;
}

void UTTGameInstance::SetNickname(const FString& _Nickname)
{
	Nickname = _Nickname;
}

FString UTTGameInstance::GetNickname() const
{
	if ( Nickname.IsEmpty() )
	{
		UE_LOG(LogTemp , Warning , TEXT("Nickname is empty , returning default value."));
		return TEXT("Empty_Nickname");
	}
	return Nickname;
}

void UTTGameInstance::SetUserId(const int32& _UserId)
{
	UserId = _UserId;
}

int32 UTTGameInstance::GetUserId() const
{
	if ( !UserId )
	{
		UE_LOG(LogTemp , Warning , TEXT("UserId is empty , returning default value."));
		return -1;
	}
	return UserId;
}

void UTTGameInstance::SetAge(const int32& _Age)
{
	Age = _Age;
}

int32 UTTGameInstance::GetAge() const
{
	return Age;
}

void UTTGameInstance::SetCoin(const int32& _Coin)
{
	Coin = _Coin;
}

void UTTGameInstance::AddCoin(int32 _Coin)
{
	Coin += _Coin;
}

int32 UTTGameInstance::GetCoin()
{
	return Coin;
}

void UTTGameInstance::SetRemainingTicketCount(const int32& _RemainingTicketCount)
{
	RemainingTicketCount = _RemainingTicketCount;
}

void UTTGameInstance::UseRemainingTicket(int32 UsedTicketCount)
{
	RemainingTicketCount -= UsedTicketCount;
}

int32 UTTGameInstance::GetRemainingTicketCount()
{
	return RemainingTicketCount;
}

void UTTGameInstance::SetAvatarData(const int32& _AvatarData)
{
	AvatarData = _AvatarData;
}

int32 UTTGameInstance::GetAvatarData()
{
	return AvatarData;
}
