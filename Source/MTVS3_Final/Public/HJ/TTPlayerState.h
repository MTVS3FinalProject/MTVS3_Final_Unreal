// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "TTPlayerState.generated.h"

/**
 *
 */
UCLASS()
class MTVS3_FINAL_API ATTPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
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
	int32 UserId;
	UFUNCTION(BlueprintCallable , Category = "Default|Authentication")
	void SetUserId(const int32& _UserId);
	long GetUserId() const;

	UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "Default|Coin")
	int32 Coin;
	UPROPERTY(BlueprintReadWrite , EditDefaultsOnly , Category = "Default|Coin")
	int32 InitialCoin = 100;
	UFUNCTION(BlueprintCallable , Category = "Default|Coin")
	void AddCoin(int32 _Coin);
	int32 GetCoin();
};
