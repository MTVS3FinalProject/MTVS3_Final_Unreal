// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "TTGameInstance.generated.h"

/**
 *
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFindSignature , bool , value);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCoinChangeSignature, int32, NewCoin);

UENUM(BlueprintType)
enum class EPlaceState : uint8
{
	Plaza = 0 ,        // 광장
	ConcertHall = 1 ,  // 콘서트홀
	LuckyDrawRoom = 2 ,      // 추첨방
	StyleLounge = 3,	// 스타일 라운지
	CommunityHall = 4,	// 커뮤니티 홀
};

UENUM(BlueprintType)
enum class ELuckyDrawState : uint8
{
	Winner = 0 ,   // 당첨
	Loser = 1 ,    // 탈락
	Neutral = 2 ,   // 아무 상태 아님
};

USTRUCT(BlueprintType)
struct FPlayerData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	bool bIsHost;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	FString nickname;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	FString titleName;
	
	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	FString titleRarity;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	FString accessToken;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	int32 coin;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	int32 avatarData;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	FString LuckyDrawSeatID;
	
	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	bool IsReceived;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	int32 ReceivedSeatId;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	bool bIsNewPlayer;
	
	// 기본 생성자
	FPlayerData()
		: bIsHost(false) , nickname(TEXT("Ticketaka")), titleName(TEXT("")), titleRarity(TEXT("Common")), accessToken(TEXT("-1")) ,
		coin(-1) , avatarData(1), LuckyDrawSeatID(TEXT("-1")), IsReceived(false), ReceivedSeatId(1), bIsNewPlayer(true)
	{}
};

UCLASS()
class MTVS3_FINAL_API UTTGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	FPlayerData PlayerData;
	FString SessionType;

public:
	// 세션 관련 델리게이트
	FFindSignature OnFindSignatureCompleteDelegate;
	virtual void Init() override;
	void ClearDestroySessionDelegate();

	FDelegateHandle OnDestroySessionCompleteDelegateHandle;
	
	// 코인 변경 델리게이트
	UPROPERTY(BlueprintAssignable, Category = "TTSettings|UserInfo")
	FCoinChangeSignature OnCoinChanged;

# pragma region 세션
	FString GenerateUniqueSessionName(const FString& SessionNamePrefix);

	// 온라인 세션 인터페이스를 기억하고 싶다.
	IOnlineSessionPtr SessionInterface;
	FString MySessionName = TEXT("TTSession");

	void FindOrCreateSession(const FString& SessionNamePrefix , int32 MaxPlayers);
	void OnFindOrCreateSessionComplete(bool bWasSuccessful);
	void AttemptJoinSession(const FOnlineSessionSearchResult& SessionResult);
	void OnMyJoinSessionComplete(FName SessionName , EOnJoinSessionCompleteResult::Type Result);
	void CreateMySession(int32 playerCount , const FString& SessionName);
	void OnMyCreateSessionComplete(FName SessionName , bool bWasSuccessful);
	void ExitSession();

	void SwitchSession(EPlaceState Destination);
	bool bSwitchToLuckyDrawSession;
	bool bSwitchToHallSession;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;;

	UFUNCTION(Server , Reliable)
	void ServerRPCExitSession();

	UFUNCTION(NetMulticast , Reliable)
	void MulticastRPCExitSession();

	// 방 퇴장 응답
	void OnMyDestroySessionComplete(FName SessionName , bool bWasSuccessful);
#pragma endregion

	UPROPERTY(EditAnywhere , Category = "TTSettings|State")
	EPlaceState PlaceState = EPlaceState::Plaza;
	UFUNCTION(BlueprintCallable , Category = "TTSettings|State")
	void SetPlaceState(EPlaceState NextPlaceState);
	UFUNCTION(BlueprintCallable , Category = "TTSettings|State")
	EPlaceState GetPlaceState() const { return PlaceState; }

	UPROPERTY(EditAnywhere , Category = "TTSettings|State")
	ELuckyDrawState LuckyDrawState = ELuckyDrawState::Neutral;
	void SetLuckyDrawState(ELuckyDrawState NextLuckyDrawState);
	UFUNCTION(BlueprintCallable , Category = "TTSettings|State")
	ELuckyDrawState GetLuckyDrawState() const { return LuckyDrawState; }
	
#pragma region Getter 및 Setter 함수
	FPlayerData GetPlayerData() const { return PlayerData; };
	void SetPlayerData(const FPlayerData& NewPlayerData) { PlayerData = NewPlayerData; };

	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetbIsHost(const bool& _bIsHost);
	bool GetbIsHost() const { return PlayerData.bIsHost; };

	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetNickname(const FString& _Nickname);
	FString GetNickname() const { return PlayerData.nickname; };

	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetTitleName(const FString& _TitleName);
	FString GetTitleName() const { return PlayerData.titleName; };
	
	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetTitleRarity(const FString& _TitleRarity);
	FString GetTitleRarity() const { return PlayerData.titleRarity; };

	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetAccessToken(const FString& _AccessToken);
	FString GetAccessToken() const { return PlayerData.accessToken; };

	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetCoin(const int32& _Coin);
	int32 GetCoin() const { return PlayerData.coin; };

	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetAvatarData(const int32& _AvatarData);
	int32 GetAvatarData() const { return PlayerData.avatarData; };

	// 추첨을 시작할 좌석 ID
	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetLuckyDrawSeatID(const FString& _LuckyDrawSeatID);
	FString GetLuckyDrawSeatID() const { return PlayerData.LuckyDrawSeatID; };

	// 좌석 접수 여부
	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetIsReceived(const bool _IsReceived);
	bool GetIsReceived() const { return PlayerData.IsReceived; };
	
	// 접수한 좌석 ID
	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetReceivedSeatId(const int32& _ReceivedSeatId);
	int32 GetReceivedSeatId() const { return PlayerData.ReceivedSeatId; };

	// 신규 플레이어 여부
	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetbIsNewPlayer(const bool _bIsNewPlayer);
	bool GetbIsNewPlayer() const { return PlayerData.bIsNewPlayer; };
#pragma endregion
};
