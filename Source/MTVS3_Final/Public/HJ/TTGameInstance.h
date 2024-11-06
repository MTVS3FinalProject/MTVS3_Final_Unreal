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

UENUM(BlueprintType)
enum class EPlaceState : uint8
{
	Plaza = 0 ,        // 광장
	ConcertHall = 1 ,  // 콘서트홀
	LuckyDrawRoom = 2 ,      // 추첨방
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
	FString title;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	FString accessToken;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	int32 coin;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	int32 remainingTicketCount;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	int32 avatarData;

	//UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	//FString ConcertName;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	FString LuckyDrawSeatID;

	// 기본 생성자
	FPlayerData()
		: bIsHost(false) , nickname(TEXT("티케타카")) , accessToken(TEXT("-1")) ,
		coin(-1) , remainingTicketCount(-1) , avatarData(1), /*ConcertName(TEXT("-1")),*/ LuckyDrawSeatID(TEXT("-1"))
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
	void SetTitle(const FString& _Title);
	FString GetTitle() const { return PlayerData.title; };

	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetAccessToken(const FString& _AccessToken);
	FString GetAccessToken() const { return PlayerData.accessToken; };

	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetCoin(const int32& _Coin);
	/*UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void AddCoin(int32 _Coin);*/
	int32 GetCoin() const { return PlayerData.coin; };

	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetRemainingTicketCount(const int32& _RemainingTicketCount);
	/*UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void UseRemainingTicket(int32 UsedTicketCount);*/
	int32 GetRemainingTicketCount() const { return PlayerData.remainingTicketCount; };

	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetAvatarData(const int32& _AvatarData);
	int32 GetAvatarData() const { return PlayerData.avatarData; };

	//UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	//void SetConcertName(const FString& _ConcertName);
	//FString GetConcertName() const { return PlayerData.ConcertName; };

	// 추첨을 시작할 좌석 ID
	UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	void SetLuckyDrawSeatID(const FString& _LuckyDrawSeatID);
	FString GetLuckyDrawSeatID() const { return PlayerData.LuckyDrawSeatID; };
#pragma endregion
};
