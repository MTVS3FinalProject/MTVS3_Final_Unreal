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
FString UTTGameInstance::GenerateUniqueSessionName(const FString& SessionNamePrefix)
{
	FDateTime Now = FDateTime::Now();
	return FString::Printf(TEXT("%s_%d%d%d%d%d") ,
		*SessionNamePrefix ,
		Now.GetHour() ,
		Now.GetMinute() ,
		Now.GetSecond() ,
		Now.GetMillisecond() ,
		FMath::Rand());
}

// 세션 검색 또는 생성 시작
void UTTGameInstance::FindOrCreateSession(const FString& SessionNamePrefix , int32 MaxPlayers)
{
	SessionType = SessionNamePrefix;  // 현재 세션 타입을 저장

	SessionSearch = MakeShareable(new FOnlineSessionSearch);
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE , true , EOnlineComparisonOp::Equals);
	SessionSearch->QuerySettings.Set(SEARCH_KEYWORDS , SessionNamePrefix , EOnlineComparisonOp::Equals);  // 접두사로 세션 검색
	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 100;

	// 세션 검색 시작
	if ( SessionInterface.IsValid() )
	{
		SessionInterface->FindSessions(0 , SessionSearch.ToSharedRef());

		// 검색 완료 후 세션 참가 처리
		SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this , &UTTGameInstance::OnFindOrCreateSessionComplete);
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

	if ( bWasSuccessful && SessionSearch->SearchResults.Num() > 0 )
	{
		UE_LOG(LogTemp , Log , TEXT("Found %d sessions:") , SessionSearch->SearchResults.Num());

		// 검색된 모든 세션 이름과 인원 수를 로그로 출력
		for ( const auto& Result : SessionSearch->SearchResults )
		{
			FString SessionName = Result.Session.SessionInfo->GetSessionId().ToString();
			int32 MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
			int32 CurrentPlayers = MaxPlayers - Result.Session.NumOpenPublicConnections;

			UE_LOG(LogTemp , Log , TEXT("Session Name: %s, Current Players: %d, Max Players: %d") ,
				   *SessionName , CurrentPlayers , MaxPlayers);
		}

		// 검색된 세션 중 참가할 수 있는 세션이 있는지 확인
		for ( const auto& Result : SessionSearch->SearchResults )
		{
			int32 CurrentPlayers = Result.Session.SessionSettings.NumPublicConnections - Result.Session.NumOpenPublicConnections;

			int32 MaxPlayers = (SessionType == TEXT("TTHallSession")) ? 100 : 30;  // 세션 종류에 따라 최대 플레이어 수 지정

			if ( CurrentPlayers < MaxPlayers )
			{
				JoinSession(Result);
				return;
			}
		}
	}

	// 참가할 세션이 없으면 새로운 세션 생성
	FString NewSessionName = GenerateUniqueSessionName(SessionType);
	CreateMySession((SessionType == TEXT("TTHallSession")) ? 100 : 30 , NewSessionName);
}

// 세션에 참가하는 함수
void UTTGameInstance::JoinSession(const FOnlineSessionSearchResult& SessionResult)
{
	if ( SessionInterface.IsValid() )
	{
		// 이미 세션에 참가했는지 확인
		if ( SessionInterface->GetNamedSession(FName(*MySessionName)) != nullptr )
		{
			UE_LOG(LogTemp , Warning , TEXT("Already in the session: %s") , *MySessionName);
			return;  // 중복 참가 방지
		}

		MySessionName = SessionResult.Session.SessionInfo->GetSessionId().ToString();
		bool bJoinSuccess = SessionInterface->JoinSession(0 , FName(*MySessionName) , SessionResult);

		if ( bJoinSuccess )
		{
			UE_LOG(LogTemp , Log , TEXT("Joining session: %s") , *MySessionName);
		}
		else
		{
			UE_LOG(LogTemp , Warning , TEXT("Failed to join session: %s") , *MySessionName);

			// 세션 참가 실패 시 새로운 유니크 세션 생성
			FString NewSessionName = GenerateUniqueSessionName(SessionType);
			CreateMySession(100 , NewSessionName);
		}
	}
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

// 새로운 세션 생성 함수
void UTTGameInstance::CreateMySession(int32 playerCount , const FString& SessionName)
{
	if ( SessionInterface->GetNamedSession(FName(*SessionName)) )
	{
		// 기존 세션이 있을 경우 삭제
		SessionInterface->DestroySession(FName(*SessionName));
	}

	FOnlineSessionSettings Settings;
	Settings.bIsDedicated = false;
	Settings.bIsLANMatch = true;
	Settings.bShouldAdvertise = true;
	Settings.bUsesPresence = true;
	Settings.bAllowJoinViaPresence = true;
	Settings.bAllowJoinInProgress = true;
	Settings.NumPublicConnections = playerCount;

	FUniqueNetIdPtr NetID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	MySessionName = SessionName;
	SessionInterface->CreateSession(*NetID , FName(*SessionName) , Settings);
}

// 세션 생성 완료 시 호출되는 함수
void UTTGameInstance::OnMyCreateSessionComplete(FName SessionName , bool bWasSuccessful)
{
	if ( bWasSuccessful )
	{
		// 세션 이름에 따라 다른 맵으로 이동
		if ( SessionType == TEXT("TTHallSession") )
		{
			GetWorld()->ServerTravel(TEXT("/Game/Ticketaka/TTHallMap?listen")); // TTHallMap으로 이동
		}
		else if ( SessionType == TEXT("TTLuckyDrawSession") )
		{
			GetWorld()->ServerTravel(TEXT("/Game/Ticketaka/TTLuckyDrawMap?listen")); // TTLuckyDrawMap으로 이동
		}
	}

	UE_LOG(LogTemp , Warning , TEXT("Create session: %s") , *SessionName.ToString());
}

void UTTGameInstance::ExitSession()
{
	ServerRPCExitSesson();
}

void UTTGameInstance::SwitchSessionToLuckyDraw()
{
	bSwitchToLuckyDrawSession = true;
	ExitSession(); // 현재 세션 나가기
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
		UE_LOG(LogTemp , Log , TEXT("Session destroyed: %s") , *SessionName.ToString());

		if ( bSwitchToLuckyDrawSession )
		{
			// SwitchSessionToLuckyDraw()가 호출된 경우
			FindOrCreateSession(TEXT("TTLuckyDrawSession") , 30);
		}
		else
		{
			// TTHallSession을 종료하고 TTLobbyMap으로 이동
			auto* pc = GetWorld()->GetFirstPlayerController();
			pc->ClientTravel(TEXT("/Game/Ticketaka/TTLobbyMap") , ETravelType::TRAVEL_Absolute);
		}
	}
	else
	{
		UE_LOG(LogTemp , Warning , TEXT("Failed to destroy session: %s") , *SessionName.ToString());
	}
	// 상태 플래그 초기화
	bSwitchToLuckyDrawSession = false;
}

#pragma region Getter 및 Setter 함수
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
#pragma endregion