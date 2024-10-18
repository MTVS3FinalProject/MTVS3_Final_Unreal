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

		// 세션 생성 완료 시 호출되는 델리게이트
		SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(
		   this , &UTTGameInstance::OnMyCreateSessionComplete);

		// 세션 검색 완료 시 호출되는 델리게이트
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(
			this , &UTTGameInstance::OnFindOrCreateSessionComplete);

		// 세션 참가 완료 시 호출되는 델리게이트
		SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(
			this , &UTTGameInstance::OnMyJoinSessionComplete);

		// 방 퇴장 응답
		SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this , &UTTGameInstance::OnMyDestroySessionComplete);
	}
}

// 유니크한 세션 이름 생성 (타임스탬프 + 랜덤 값)
FString UTTGameInstance::GenerateUniqueSessionName()
{
	FDateTime Now = FDateTime::Now();
	return FString::Printf(TEXT("Session_%d%d%d%d%d") ,
		Now.GetHour() , Now.GetMinute() , Now.GetSecond() , Now.GetMillisecond() , FMath::Rand());
}

// 세션 검색 또는 생성 시작
void UTTGameInstance::FindOrCreateSession()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE , true , EOnlineComparisonOp::Equals);
	SessionSearch->bIsLanQuery = true; // LAN 설정 활성화 (같은 컴퓨터 테스트 시 필수)
	SessionSearch->MaxSearchResults = 100;

	// 세션 검색 시작
	SessionInterface->FindSessions(0 , SessionSearch.ToSharedRef());

	// UI에 세션 검색 시작 알림
	if ( OnFindSignatureCompleteDelegate.IsBound() )
	{
		OnFindSignatureCompleteDelegate.Broadcast(true);
	}
}

// 세션 검색 완료 시 호출되는 함수
void UTTGameInstance::OnFindOrCreateSessionComplete(bool bWasSuccessful)
{
	// UI에 검색 완료 알림
	if ( OnFindSignatureCompleteDelegate.IsBound() )
	{
		OnFindSignatureCompleteDelegate.Broadcast(false);
	}

	UE_LOG(LogTemp , Log , TEXT("Session search complete. Success: %s, Results: %d") ,
		   bWasSuccessful ? TEXT("true") : TEXT("false") ,
		   SessionSearch->SearchResults.Num());

	// 세션 검색이 완료되면 참가 시도
	AttemptJoinSession();
}

// 세션에 참가 시도 (인원 초과 시 새로운 세션 생성)
void UTTGameInstance::AttemptJoinSession()
{
	if ( SessionSearch.IsValid() && SessionSearch->SearchResults.Num() > 0 )
	{
		for ( const auto& Result : SessionSearch->SearchResults )
		{
			int32 CurrentPlayers = Result.Session.SessionSettings.NumPublicConnections -
				Result.Session.NumOpenPublicConnections;

			UE_LOG(LogTemp , Log , TEXT("Found session with %d/%d players.") ,
				   CurrentPlayers , Result.Session.SessionSettings.NumPublicConnections);

			// 인원이 100명 미만이면 해당 세션에 참가
			if ( CurrentPlayers < 100 )
			{
				JoinSession(Result);
				return;
			}
		}
	}

	// 모든 세션이 가득 찼다면 새로운 세션 생성
	FString NewSessionName = GenerateUniqueSessionName();
	CreateMySession(100 , NewSessionName);
}

// 세션에 참가하는 함수
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

			// 세션 참가 실패 시 새로운 유니크 세션 생성
			FString NewSessionName = GenerateUniqueSessionName();
			CreateMySession(100 , NewSessionName);
		}
	}
}

// 새로운 세션 생성 함수
void UTTGameInstance::CreateMySession(int32 playerCount , const FString& SessionName)
{
	if ( SessionInterface->GetNamedSession(FName(*SessionName)) )
	{
		// 기존 세션이 있을 경우 삭제
		SessionInterface->DestroySession(FName(*SessionName));
	}

	FOnlineSessionSettings Settings;
	Settings.bIsDedicated = false;  // 리슨 서버 설정
	Settings.bIsLANMatch = true;    // LAN 설정
	Settings.bShouldAdvertise = true;
	Settings.bUsesPresence = true;
	Settings.bAllowJoinViaPresence = true;
	Settings.bAllowJoinInProgress = true;
	Settings.NumPublicConnections = playerCount;

	FUniqueNetIdPtr NetID = GetWorld()->GetFirstLocalPlayerFromController()
		->GetUniqueNetIdForPlatformUser()
		.GetUniqueNetId();

	// 새로운 세션 생성
	SessionInterface->CreateSession(*NetID , FName(*SessionName) , Settings);
}

// 세션 생성 완료 시 호출되는 함수
void UTTGameInstance::OnMyCreateSessionComplete(FName SessionName , bool bWasSuccessful)
{
	if ( bWasSuccessful )
	{
		// 리슨 서버로 맵 이동
		GetWorld()->ServerTravel(TEXT("/Game/Ticketaka/TTHallMap?listen"));
		//GetWorld()->ServerTravel(TEXT("/Game/KHJ/Maps/HJProtoMap?listen"));
	}
	UE_LOG(LogTemp , Warning , TEXT("Create session : % s") , *MySessionName);
}

// 세션 참가 완료 시 호출되는 함수
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

void UTTGameInstance::ExitSession()
{
	ServerRPCExitSesson();
}

void UTTGameInstance::ServerRPCExitSesson_Implementation()
{
	MulticastRPCExitSession();
}

void UTTGameInstance::MulticastRPCExitSession_Implementation()
{
	// 방 퇴장 요청
	SessionInterface->DestroySession(FName(MySessionName));
}

void UTTGameInstance::OnMyDestroySessionComplete(FName SessionName , bool bWasSuccessful)
{
	if ( bWasSuccessful )
	{
		// 클라이언트가 로비로 여행을 가고 싶다.
		auto* pc = GetWorld()->GetFirstPlayerController();
		pc->ClientTravel(TEXT("/Game/Ticketaka/TTLobbyMap") , ETravelType::TRAVEL_Absolute);
	}
}

void UTTGameInstance::SetPlayerData(const FPlayerData& NewPlayerData)
{
	PlayerData = NewPlayerData;
}

FPlayerData UTTGameInstance::GetPlayerData() const
{
	return PlayerData;
}

void UTTGameInstance::SetbIsHost(const bool& _bIsHost)
{
	PlayerData.bIsHost = _bIsHost;
	SetPlayerData(PlayerData);
}

bool UTTGameInstance::GetbIsHost() const
{
	return PlayerData.bIsHost;
}

void UTTGameInstance::SetNickname(const FString& _Nickname)
{
	PlayerData.Nickname = _Nickname;
	SetPlayerData(PlayerData);
}

FString UTTGameInstance::GetNickname() const
{
	if ( PlayerData.Nickname.IsEmpty() )
	{
		UE_LOG(LogTemp , Warning , TEXT("Nickname is empty , returning default value."));
		return TEXT("티케타카");
	}
	return PlayerData.Nickname;
}

void UTTGameInstance::SetAccessToken(const FString& _AccessToken)
{
	PlayerData.AccessToken = _AccessToken;
	SetPlayerData(PlayerData);
}

FString UTTGameInstance::GetAccessToken() const
{
	return PlayerData.AccessToken;
}

void UTTGameInstance::SetBirth(const FString& _Birth)
{
	PlayerData.Birth = _Birth;
	SetPlayerData(PlayerData);
}

FString UTTGameInstance::GetBirth() const
{
	return PlayerData.Birth;
}

void UTTGameInstance::SetCoin(const int32& _Coin)
{
	PlayerData.Coin = _Coin;
	SetPlayerData(PlayerData);
}

void UTTGameInstance::AddCoin(int32 _Coin)
{
	PlayerData.Coin += _Coin;
	SetPlayerData(PlayerData);
}

int32 UTTGameInstance::GetCoin()
{
	return PlayerData.Coin;
}

void UTTGameInstance::SetRemainingTicketCount(const int32& _RemainingTicketCount)
{
	PlayerData.RemainingTicketCount = _RemainingTicketCount;
	SetPlayerData(PlayerData);
}

void UTTGameInstance::UseRemainingTicket(int32 UsedTicketCount)
{
	PlayerData.RemainingTicketCount -= UsedTicketCount;
	SetPlayerData(PlayerData);
}

int32 UTTGameInstance::GetRemainingTicketCount()
{
	return PlayerData.RemainingTicketCount;
}

void UTTGameInstance::SetAvatarData(const int32& _AvatarData)
{
	PlayerData.AvatarData = _AvatarData;
	SetPlayerData(PlayerData);
}

int32 UTTGameInstance::GetAvatarData()
{
	return PlayerData.AvatarData;
}