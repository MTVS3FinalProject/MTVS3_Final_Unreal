// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TTHallGameState.generated.h"

/**
 *
 */
UCLASS()
class MTVS3_FINAL_API ATTHallGameState : public AGameState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

public:
	void SendLuckyDrawInvitation(const TArray<FString>& NicknameList, int32 CompetitionRate);

	UPROPERTY()
	FTimerHandle HideLDInvitationTimerHandle;
	// void HideLuckyDrawInvitation(const TArray<FString>& NicknameList, int32 CompetitionRate);

	// 모든 의자의 상태를 한 번에 업데이트
	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateChairStates(const TArray<int32>& ReservedSeats);

	// 의자 상태가 변경될 때마다 호출될 이벤트
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChairStatesUpdated);
    
	UPROPERTY(BlueprintAssignable, Category = "Chair")
	FOnChairStatesUpdated OnChairStatesUpdated;

	// 예약된 좌석 ID들을 저장
	UPROPERTY(Replicated)
	TArray<int32> ReservedSeatIds;

	UPROPERTY()
	TSet<int32> ReservedSeatIdsSet;

	// TSet을 반환하는 getter
	const TSet<int32>& GetReservedSeatIdsSet() const { return ReservedSeatIdsSet; }
};
