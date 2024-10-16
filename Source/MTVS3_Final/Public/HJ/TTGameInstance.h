﻿// Fill out your copyright notice in the Description page of Project Settings.

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

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|Authentication")
	bool bIsHost;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|Authentication")
	FString Nickname;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|Authentication")
	FString AccessToken;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|Authentication")
	FString Birth;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|Cash")
	int32 Coin;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|Cash")
	int32 RemainingTicketCount;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|Avatar")
	int32 AvatarData;

	// 기본 생성자
	FPlayerData()
		: bIsHost(false) , Nickname(TEXT("티케타카")) , AccessToken(TEXT("")) , Birth(TEXT("")) ,
		Coin(0) , RemainingTicketCount(0) , AvatarData(0)
	{}
};

UCLASS()
class MTVS3_FINAL_API UTTGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	FPlayerData PlayerData;
public:
	// 세션 관련 델리게이트
	FFindSignature OnFindSignatureCompleteDelegate;

	virtual void Init() override;

	FString GenerateUniqueSessionName();

	// 온라인 세션 인터페이스를 기억하고 싶다.
	IOnlineSessionPtr SessionInterface;
	FString MySessionName = TEXT("TTHallSession");

	void FindOrCreateSession();
	void OnFindOrCreateSessionComplete(bool bWasSuccessful);
	void AttemptJoinSession();
	void CreateMySession(int32 playerCount , const FString& SessionNam);
	void OnMyCreateSessionComplete(FName SessionName , bool bWasSuccessful);
	void JoinSession(const FOnlineSessionSearchResult& SessionResult);
	void OnMyJoinSessionComplete(FName SessionName , EOnJoinSessionCompleteResult::Type Result);

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

	// 방 퇴장 요청
	void ExitSession();

	UFUNCTION(Server, Reliable)
	void ServerRPCExitSesson();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCExitSession();

	// 방 퇴장 응답
	void OnMyDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	// Getter 및 Setter 함수
	FPlayerData GetPlayerData() const;
	void SetPlayerData(const FPlayerData& NewPlayerData);

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|Authentication")
	bool bIsHost;
	UFUNCTION(BlueprintCallable , Category = "Default|Authentication")
	void SetbIsHost(const bool& _bIsHost);
	bool GetbIsHost() const;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|Authentication")
	FString Nickname;
	UFUNCTION(BlueprintCallable , Category = "Default|Authentication")
	void SetNickname(const FString& _Nickname);
	FString GetNickname() const;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|Authentication")
	FString AccessToken;
	UFUNCTION(BlueprintCallable , Category = "Default|Authentication")
	void SetAccessToken(const FString& _AccessToken);
	FString GetAccessToken() const;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|Authentication")
	FString Birth;
	UFUNCTION(BlueprintCallable , Category = "Default|Authentication")
	void SetBirth(const FString& _Birth);
	FString GetBirth() const;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|Cash")
	int32 Coin;
	UFUNCTION(BlueprintCallable , Category = "Default|Cash")
	void SetCoin(const int32& _Coin);
	UFUNCTION(BlueprintCallable , Category = "Default|Cash")
	void AddCoin(int32 _Coin);
	int32 GetCoin();

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|Cash")
	int32 RemainingTicketCount;
	UFUNCTION(BlueprintCallable , Category = "Default|Cash")
	void SetRemainingTicketCount(const int32& _RemainingTicketCount);
	UFUNCTION(BlueprintCallable , Category = "Default|Cash")
	void UseRemainingTicket(int32 UsedTicketCount);
	int32 GetRemainingTicketCount();

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|Avatar")
	int32 AvatarData;
	UFUNCTION(BlueprintCallable , Category = "Default|Avatar")
	void SetAvatarData(const int32& _AvatarData);
	int32 GetAvatarData();
};
