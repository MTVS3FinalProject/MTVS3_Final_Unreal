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

USTRUCT(BlueprintType)
struct FPlayerData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|UserInfo")
	bool bIsHost;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|UserInfo")
	FString Nickname;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|UserInfo")
	FString AccessToken;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|UserInfo")
	int32 Coin;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|UserInfo")
	int32 RemainingTicketCount;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|UserInfo")
	int32 AvatarData;

	// 기본 생성자
	FPlayerData()
		: bIsHost(false) , Nickname(TEXT("티케타카")) , AccessToken(TEXT("")) ,
		Coin(0) , RemainingTicketCount(0) , AvatarData(0)
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

	FString GenerateUniqueSessionName(const FString& SessionNamePrefix);

	// 온라인 세션 인터페이스를 기억하고 싶다.
	IOnlineSessionPtr SessionInterface;
	FString MySessionName = TEXT("TTSession");

	void FindOrCreateSession(const FString& SessionNamePrefix , int32 MaxPlayers);
	void OnFindOrCreateSessionComplete(bool bWasSuccessful);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void OnMyJoinSessionComplete(FName SessionName , EOnJoinSessionCompleteResult::Type Result);
	void CreateMySession(int32 playerCount , const FString& SessionName);
	void OnMyCreateSessionComplete(FName SessionName , bool bWasSuccessful);
	void ExitSession();

	void SwitchSessionToLuckyDraw();
	bool bSwitchToLuckyDrawSession;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;;

	UFUNCTION(Server , Reliable)
	void ServerRPCExitSesson();

	UFUNCTION(NetMulticast , Reliable)
	void MulticastRPCExitSession();

	// 방 퇴장 응답
	void OnMyDestroySessionComplete(FName SessionName , bool bWasSuccessful);

#pragma region Getter 및 Setter 함수
	FPlayerData GetPlayerData() const;
	void SetPlayerData(const FPlayerData& NewPlayerData);

	UFUNCTION(BlueprintCallable , Category = "Default|UserInfo")
	void SetbIsHost(const bool& _bIsHost);
	bool GetbIsHost() const;

	UFUNCTION(BlueprintCallable , Category = "Default|UserInfo")
	void SetNickname(const FString& _Nickname);
	FString GetNickname() const;

	UFUNCTION(BlueprintCallable , Category = "Default|UserInfo")
	void SetAccessToken(const FString& _AccessToken);
	FString GetAccessToken() const;

	UFUNCTION(BlueprintCallable , Category = "Default|UserInfo")
	void SetCoin(const int32& _Coin);
	UFUNCTION(BlueprintCallable , Category = "Default|UserInfo")
	void AddCoin(int32 _Coin);
	int32 GetCoin();

	UFUNCTION(BlueprintCallable , Category = "Default|UserInfo")
	void SetRemainingTicketCount(const int32& _RemainingTicketCount);
	UFUNCTION(BlueprintCallable , Category = "Default|UserInfo")
	void UseRemainingTicket(int32 UsedTicketCount);
	int32 GetRemainingTicketCount();

	UFUNCTION(BlueprintCallable , Category = "Default|UserInfo")
	void SetAvatarData(const int32& _AvatarData);
	int32 GetAvatarData();
#pragma endregion
};
