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
	void SendLuckyDrawInvitation(const TArray<FString>& NicknameList, const FString& SeatInfo, int32 CompetitionRate);

	UPROPERTY()
	FTimerHandle HideLDInvitationTimerHandle;
	// void HideLuckyDrawInvitation(const TArray<FString>& NicknameList, int32 CompetitionRate);

	// 모든 의자의 상태를 한 번에 업데이트
	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateChairStates(const TArray<int32>& ReservedSeats);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAddReservedSeats(const TArray<int32>& NewReservedSeats);

	// 의자 상태가 변경될 때마다 호출될 이벤트
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChairStatesUpdated);
    
	UPROPERTY(BlueprintAssignable, Category = "Chair")
	FOnChairStatesUpdated OnChairStatesUpdated;

	// 예약된 좌석 ID들을 저장
	UPROPERTY(Replicated)
	TArray<int32> ReservedSeatIds;

	const TArray<int32>& GetReservedSeatIds() const {return ReservedSeatIds; }

	UPROPERTY()
	TSet<int32> ReservedSeatIdsSet;

	// TSet을 반환하는 getter
	const TSet<int32>& GetReservedSeatIdsSet() const { return ReservedSeatIdsSet; }

	// 추첨장 좌석 ID 설정 함수
	UFUNCTION()
	void SetLuckyDrawSeatId(const FString& NewSeatId);

	// 추첨장 좌석 ID 반환 함수
	UFUNCTION()
	const FString& GetLuckyDrawSeatId() const { return LuckyDrawSeatId; }

	UPROPERTY(Replicated)
	FString LuckyDrawSeatId;

	
public:
	// 트리 티켓 동기화
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnTicketImageUpdated, int32, TicketIndex, FString, TicketImgUrl);

	UPROPERTY(BlueprintAssignable, Category = "Tickets")
	FOnTicketImageUpdated OnTicketImageUpdated;
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ApplyTicketImage(int32 TicketIndex, const FString& TicketImgUrl);
};
