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

FString UTTGameInstance::GenerateUniqueSessionName()
{
	// 유니크한 세션 이름 생성 (타임스탬프 + 랜덤 값 사용)
	FDateTime Now = FDateTime::Now();
	return FString::Printf(TEXT("Session_%d%d%d%d%d") ,
		Now.GetHour() , Now.GetMinute() , Now.GetSecond() , Now.GetMillisecond() , FMath::Rand());
}

void UTTGameInstance::FindOrCreateSession()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE , true , EOnlineComparisonOp::Equals);
	SessionSearch->bIsLanQuery = true; // LAN 설정 활성화 (같은 컴퓨터 테스트 시 필수)
	SessionSearch->MaxSearchResults = 100;

	SessionInterface->FindSessions(0 , SessionSearch.ToSharedRef());

	if ( OnFindSignatureCompleteDelegate.IsBound() )
	{
		OnFindSignatureCompleteDelegate.Broadcast(true);  // 탐색 시작 UI 업데이트
	}
}

void UTTGameInstance::OnFindOrCreateSessionComplete(bool bWasSuccessful)
{
	if ( OnFindSignatureCompleteDelegate.IsBound() )
	{
		OnFindSignatureCompleteDelegate.Broadcast(false);  // UI: 검색 완료 알림
	}

	UE_LOG(LogTemp , Log , TEXT("Session search complete. Success: %s, Results: %d") ,
		   bWasSuccessful ? TEXT("true") : TEXT("false") ,
		   SessionSearch->SearchResults.Num());

	// 세션 검색이 완료되면 참가 시도
	AttemptJoinSession();

	//if ( bWasSuccessful && SessionSearch->SearchResults.Num() > 0 )
	//{
	//	for ( const auto& Result : SessionSearch->SearchResults )
	//	{
	//		// 기존 세션이 있으면 참가
	//		if ( Result.Session.SessionInfo->GetSessionId().ToString() == MySessionName )
	//		{
	//			JoinSession(Result);
	//			return;
	//		}
	//	}
	//}

	//// 유효한 세션이 없으면 새로운 세션 생성
	//CreateMySession(100);
}

void UTTGameInstance::AttemptJoinSession()
{
	if ( SessionSearch.IsValid() && SessionSearch->SearchResults.Num() > 0 )
	{
		// 첫 번째 세션에 참가 시도
		UE_LOG(LogTemp , Log , TEXT("Attempting to join session..."));
		JoinSession(SessionSearch->SearchResults[0]);
	}
	else
	{
		// 세션이 없으면 새로 생성
		UE_LOG(LogTemp , Log , TEXT("No sessions found. Creating a new session..."));
		CreateMySession(100);
	}
}

void UTTGameInstance::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if ( SessionInterface.IsValid() )
	{
		bool bJoinSuccess = SessionInterface->JoinSession(0 , FName(MySessionName) , SessionResult);

		if ( bJoinSuccess )
		{
			UE_LOG(LogTemp , Log , TEXT("Joining session: %s") , *MySessionName);
		}
		else
		{
			UE_LOG(LogTemp , Warning , TEXT("Failed to join session: %s") , *MySessionName);
			CreateMySession(100);  // 실패 시 세션 생성
		}
	}
}


void UTTGameInstance::CreateMySession(int32 playerCount)
{
	if ( SessionInterface->GetNamedSession(FName(MySessionName)) )
	{
		SessionInterface->DestroySession(FName(MySessionName));
	}

	FOnlineSessionSettings Settings;
	Settings.bIsDedicated = false;
	Settings.bIsLANMatch = true;
	Settings.bShouldAdvertise = true;
	Settings.bUsesPresence = true;
	Settings.bAllowJoinViaPresence = true;
	Settings.bAllowJoinInProgress = true;
	Settings.NumPublicConnections = playerCount;

	FUniqueNetIdPtr NetID = GetWorld()->GetFirstLocalPlayerFromController()
		->GetUniqueNetIdForPlatformUser()
		.GetUniqueNetId();

	SessionInterface->CreateSession(*NetID , FName(MySessionName) , Settings);
}

void UTTGameInstance::OnMyCreateSessionComplete(FName SessionName , bool bWasSuccessful)
{
	//// 로딩 완료 시 UI에 알림
	//if ( OnFindSignatureCompleteDelegate.IsBound() )
	//{
	//	OnFindSignatureCompleteDelegate.Broadcast(false);  // 로딩 종료
	//}

	if ( bWasSuccessful )
	{
		// 리슨 서버로 맵 이동
		//GetWorld()->ServerTravel(TEXT("/Game/Ticketaka/TTHallMap?listen"));
		GetWorld()->ServerTravel(TEXT("/Game/KHJ/Maps/HJProtoMap?listen"));
	}
	UE_LOG(LogTemp , Warning , TEXT("Create session : % s") , *MySessionName);
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
