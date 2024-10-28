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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

public:	
	//UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	//bool bIsHost;
	//UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	//void SetbIsHost(const bool& _bIsHost) { bIsHost = _bIsHost; };
	//bool GetbIsHost() const { return bIsHost; };

	//UPROPERTY(ReplicatedUsing = OnRep_Nickname, VisibleAnywhere , Category = "TTSettings|UserInfo")
	//FString Nickname;
	//UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	//void SetNickname(const FString& _Nickname);
	//FString GetNickname() const { return Nickname; };

	//UFUNCTION(Server , Unreliable)
	//void ServerSetNickname();

	//UFUNCTION()
	//void OnRep_Nickname();
	//FTimerHandle TimerHandle_OnRepRetry;

	//// 추첨을 시작할 좌석 ID
	//UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	//FString LuckyDrawSeatID;
	//UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	//void SetLuckyDrawSeatID(const FString& _LuckyDrawSeatID);
	//FString GetLuckyDrawSeatID() const { return LuckyDrawSeatID; };

	//// 랜덤으로 배치된 좌석 번호
	//UPROPERTY(BlueprintReadWrite , VisibleAnywhere , Category = "TTSettings|UserInfo")
	//int32 RandomSeatNumber = -1;
	//UFUNCTION(BlueprintCallable , Category = "TTSettings|UserInfo")
	//void SetRandomSeatNumber(const int32& _RandomSeatNumber);
	//int32 GetRandomSeatNumber() const { return RandomSeatNumber; }
};
