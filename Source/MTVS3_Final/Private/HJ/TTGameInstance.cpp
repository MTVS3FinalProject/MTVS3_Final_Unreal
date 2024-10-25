// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"
#include "HJ/TTPlayerState.h"

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

#pragma region 세션
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

	SessionSearch->bIsLanQuery = true;
	SessionSearch->MaxSearchResults = 100;

	// 세션 검색 시작
	if ( SessionInterface.IsValid() )
	{
		SessionInterface->FindSessions(0 , SessionSearch.ToSharedRef());

		//// 검색 완료 후 세션 참가 처리
		//SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this , &UTTGameInstance::OnFindOrCreateSessionComplete);
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
			FString SessionName;
			if ( Result.Session.SessionSettings.Get(FName("SessionName") , SessionName) )  // 세션 이름 가져오기
			{
				int32 MaxPlayers = Result.Session.SessionSettings.NumPublicConnections;
				int32 CurrentPlayers = MaxPlayers - Result.Session.NumOpenPublicConnections;

				UE_LOG(LogTemp , Log , TEXT("Session Name: %s, Current Players: %d, Max Players: %d") ,
					   *SessionName , CurrentPlayers , MaxPlayers);

				// 세션 이름 접두사로 필터링
				if ( SessionName.StartsWith(SessionType) )
				{
					JoinSession(Result);
					return;
				}
			}
		}

		UE_LOG(LogTemp , Warning , TEXT("No session with prefix: %s was found.") , *SessionType);
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
		FString FoundSessionName;
		if ( SessionResult.Session.SessionSettings.Get(FName("SessionName") , FoundSessionName) )
		{
			MySessionName = FoundSessionName;
		}
		else
		{
			UE_LOG(LogTemp , Warning , TEXT("SessionName not found in session settings."));
			return;
		}

		// 이미 세션에 참가했는지 확인
		if ( SessionInterface->GetNamedSession(FName(*MySessionName)) != nullptr )
		{
			UE_LOG(LogTemp , Warning , TEXT("Already in the session: %s") , *MySessionName);
			return;  // 중복 참가 방지
		}

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
			// 세션 참가 성공 시 레벨 이동
			PlayerController->ClientTravel(URL , ETravelType::TRAVEL_Absolute);
			UE_LOG(LogTemp , Log , TEXT("Successfully joined session and travelling to URL: %s") , *URL);
		}
	}
	else
	{
		UE_LOG(LogTemp , Warning , TEXT("Failed to join session: %s") , *SessionName.ToString());
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

	//// 세션 이름 저장
	//Settings.Set(FName("SessionName") , MySessionName , EOnlineDataAdvertisementType::ViaOnlineService);

	//FUniqueNetIdPtr NetID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	//MySessionName = SessionName;
	//SessionInterface->CreateSession(*NetID , FName(*SessionName) , Settings);

	MySessionName = SessionName;
	Settings.Set(FName("SessionName") , MySessionName , EOnlineDataAdvertisementType::ViaOnlineService);

	FUniqueNetIdPtr NetID = GetWorld()->GetFirstLocalPlayerFromController()->GetUniqueNetIdForPlatformUser().GetUniqueNetId();

	// 세션 생성 요청
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
			if ( bEnableServerTravel )
			{
				// 클라이언트 같이 이동시키고 싶은 경우
				GetWorld()->ServerTravel(TEXT("/Game/Ticketaka/TTLuckyDrawMap?listen"));
			}
			else
			{
				// 홀 관리자 1, 추첨방 관리자 1 해서 각자 이동하고 싶은 경우
				auto* pc = GetWorld()->GetFirstPlayerController();
				pc->ClientTravel(TEXT("/Game/Ticketaka/TTLuckyDrawMap") , ETravelType::TRAVEL_Absolute); // TTLuckyDrawMap으로 이동
				SetPlaceState(EPlaceState::LuckyDrawRoom);
			}
		}
	}

	UE_LOG(LogTemp , Warning , TEXT("Create session: %s") , *SessionName.ToString());
}

void UTTGameInstance::ExitSession()
{
	ServerRPCExitSesson();
}

void UTTGameInstance::SwitchSession(EPlaceState Destination)
{
	switch ( Destination )
	{
		// 목적지가 추첨방이면
	case EPlaceState::LuckyDrawRoom:
		bSwitchToLuckyDrawSession = true;
		break;
		// 목적지가 광장 또는 콘서트홀이면(광장에 스폰)
	case EPlaceState::Plaza:
	case EPlaceState::ConcertHall:
		bSwitchToHallSession = true;
		break;
	}

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
			FindOrCreateSession(TEXT("TTLuckyDrawSession") , 30);
		}
		else if ( bSwitchToHallSession )
		{
			FindOrCreateSession(TEXT("TTHallSession") , 100);
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
	bSwitchToHallSession = false;
}
#pragma endregion

void UTTGameInstance::SetPlaceState(EPlaceState NextPlaceState)
{
	EPlaceState PrevPlaceState = PlaceState;
	PlaceState = NextPlaceState;
}

void UTTGameInstance::SetLuckyDrawState(ELuckyDrawState NextLuckyDrawState)
{
	ELuckyDrawState PrevLuckyDrawState = LuckyDrawState;
	LuckyDrawState = NextLuckyDrawState;
}

#pragma region Getter 및 Setter 함수
void UTTGameInstance::SetbIsHost(const bool& _bIsHost)
{
	PlayerData.bIsHost = _bIsHost;
	SetPlayerData(PlayerData);
}

void UTTGameInstance::SetNickname(const FString& _Nickname)
{
	PlayerData.Nickname = _Nickname;
	SetPlayerData(PlayerData);
}

void UTTGameInstance::SetAccessToken(const FString& _AccessToken)
{
	PlayerData.AccessToken = _AccessToken;
	SetPlayerData(PlayerData);
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

void UTTGameInstance::SetAvatarData(const int32& _AvatarData)
{
	PlayerData.AvatarData = _AvatarData;
	SetPlayerData(PlayerData);
}

void UTTGameInstance::SetConcertName(const FString& _ConcertName)
{
	PlayerData.ConcertName = _ConcertName;
	SetPlayerData(PlayerData);
}

void UTTGameInstance::SetLuckyDrawSeatID(const FString& _LuckyDrawSeatID)
{
	PlayerData.LuckyDrawSeatID = _LuckyDrawSeatID;
	SetPlayerData(PlayerData);
}
#pragma endregion