// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

void UTTGameInstance::Init()
{
	Super::Init();

	if ( auto* subSystem = IOnlineSubsystem::Get() )
	{
		SessionInterface = subSystem->GetSessionInterface();

		// 방 생성 요청 -> 응답
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(
		   this , &UTTGameInstance::OnMyCreateSessionComplete);

		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(
			this , &UTTGameInstance::OnFindOrCreateSessionComplete);

		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(
			this , &UTTGameInstance::OnMyJoinSessionComplete);
	}
}

// 유니크한 세션 이름 생성 함수 (세션 이름에 타임스탬프 추가)
FString GenerateUniqueSessionName()
{
	FDateTime Now = FDateTime::Now();
	return FString::Printf(TEXT("Session_%d%d%d%d%d") , Now.GetHour() , Now.GetMinute() , Now.GetSecond() , Now.GetMillisecond() , FMath::Rand());
}

void UTTGameInstance::FindOrCreateSession()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE , true , EOnlineComparisonOp::Equals);
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 100;

	SessionInterface->FindSessions(0 , SessionSearch.ToSharedRef());

	// 탐색 시작 시 UI에 알림
	if ( OnFindSignatureCompleteDelegate.IsBound() )
	{
		OnFindSignatureCompleteDelegate.Broadcast(true); // 탐색 중 상태 전송
	}
}

void UTTGameInstance::OnFindOrCreateSessionComplete(bool bWasSuccessful)
{
	if ( bWasSuccessful && SessionSearch->SearchResults.Num() > 0 )
	{
		for ( const auto& Result : SessionSearch->SearchResults )
		{
			int32 CurrentPlayers = Result.Session.SessionSettings.NumPublicConnections -
				Result.Session.NumOpenPublicConnections;

			// 세션에 사람이 없다면 파괴하고 새로 생성
			if ( CurrentPlayers == 0 )
			{
				DestroyExistingSession(Result.Session.OwningUserName);
				CreateMySession(100);
				return;
			}

			// 사람이 있는 세션이 있다면 해당 세션에 참가
			if ( CurrentPlayers < 100 )
			{
				JoinSession(Result);
				return;
			}
		}
	}

	// 기존 세션에 참가하지 못한 경우, 새로운 세션을 유니크한 이름으로 생성
	MySessionName = GenerateUniqueSessionName();
	CreateMySession(100);
}

void UTTGameInstance::DestroyExistingSession(const FString& SessionName)
{
	if ( SessionInterface->GetNamedSession(FName(*SessionName)) )
	{
		SessionInterface->DestroySession(FName(*SessionName));
	}
}

void UTTGameInstance::CreateMySession(int32 playerCount)
{
	FOnlineSessionSettings Settings;
	Settings.bIsDedicated = false;
	Settings.bIsLANMatch = IOnlineSubsystem::Get()->GetSubsystemName() == "NULL";
	Settings.bShouldAdvertise = true;
	Settings.bUsesPresence = true;
	Settings.bAllowJoinViaPresence = true;
	Settings.bAllowJoinInProgress = true;
	Settings.NumPublicConnections = playerCount;

	FUniqueNetIdPtr NetID = GetWorld()->GetFirstLocalPlayerFromController()
		->GetUniqueNetIdForPlatformUser()
		.GetUniqueNetId();

	// 새로운 세션 생성
	SessionInterface->CreateSession(*NetID , FName(MySessionName) , Settings);
}

void UTTGameInstance::OnMyCreateSessionComplete(FName SessionName , bool bWasSuccessful)
{
	// 로딩 완료 시 UI에 알림
	if ( OnFindSignatureCompleteDelegate.IsBound() )
	{
		OnFindSignatureCompleteDelegate.Broadcast(false);  // 로딩 종료
	}

	if ( bWasSuccessful )
	{
		//GetWorld()->ServerTravel(TEXT("/Game/Ticketaka/TTHallMap?listen"));
		GetWorld()->ServerTravel(TEXT("/Game/KHJ/Maps/HJProtoMap?listen"));
	}
}

void UTTGameInstance::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if ( SessionInterface.IsValid() )
	{
		SessionInterface->JoinSession(0 , FName(MySessionName) , SessionResult);
	}
}

void UTTGameInstance::OnMyJoinSessionComplete(FName SessionName , EOnJoinSessionCompleteResult::Type Result)
{
	if ( Result == EOnJoinSessionCompleteResult::Success )
	{
		auto* PlayerController = GetWorld()->GetFirstPlayerController();
		FString URL;
		if ( SessionInterface->GetResolvedConnectString(SessionName , URL) && !URL.IsEmpty() )
		{
			PlayerController->ClientTravel(URL , ETravelType::TRAVEL_Absolute);
		}
	}
}

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
