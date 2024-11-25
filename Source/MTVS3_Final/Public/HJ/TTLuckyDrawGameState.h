// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TTLuckyDrawGameState.generated.h"

/**
 *
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRequestMovePlayersToChairs);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoundEndDelegate);

UCLASS()
class MTVS3_FINAL_API ATTLuckyDrawGameState : public AGameState
{
	GENERATED_BODY()

protected:
	ATTLuckyDrawGameState();
	
	virtual void BeginPlay() override;

	// GameUI 초기화 상태를 추적하는 변수 추가
	bool bIsGameUIInitialized = false;

public:
	// 5초 카운트다운 후 룰렛 돌리기 시작하는 딜레이
	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Timer")
	float StartPlayRouletteDelayTime = 6.0f;

	// 룰렛 처음으로 돌리는 딜레이
	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Timer")
	float FirstPlayRouletteDelayTime = 0.5f;

	// 두 번째 이후의 룰렛 돌리는 릴레이
	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Timer")
	float PlayRouletteDelayTime = 10.0f;

	// 플레이어 탈락 딜레이
	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Timer")
	float EliminatePlayersDelayTime = 4.5f;

	// 결과 나오고 우승자 시퀀스 나올 때까지의 딜레이
	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Timer")
	float EndRoundsDelayTime = 8.0f;

	// 랜덤으로 떨어지는 최대 딜레이
	UPROPERTY(EditDefaultsOnly , Category = "TTSettings|Timer")
	float MaxRandomFallDelay = 1.5f;
	
	UFUNCTION(BlueprintCallable)
	void ResetChair();
	
#pragma region UI
	UPROPERTY(EditAnywhere , Category = "TTSettings|UI")
	TSubclassOf<class UMH_GameWidget> GameUIFactory;
	UPROPERTY()
	class UMH_GameWidget* GameUI;
#pragma endregion
	UPROPERTY(EditAnywhere , Category = "TTSettings|Debug")
	bool bShowDebug = false;
	
	UPROPERTY(BlueprintAssignable , Category = "Events")
	FOnRequestMovePlayersToChairs OnRequestMovePlayersToChairs;

	void AssignSeatNumber(APlayerState* PlayerState);
	void StartLuckyDraw();

	UFUNCTION()
	void MovePlayersToChairs();

	int32 CurrentSeatNumber = 1;

	UPROPERTY(ReplicatedUsing=OnRep_NewSeatNumber)
	int32 NewSeatNumber = -1;

	void StartRounds(int32 InTotalRounds);
	void StartNextRound();

	void PlayRoulette();

	void StartPlayRoulette();

	void EndRounds();
	void InitializeChairs();
	
	bool bIsStartRound = false;

	void EliminatePlayers();

#pragma region 멀티플레이
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnRep_NewSeatNumber();

	UFUNCTION(NetMulticast , Reliable)
	void MulticastUpdatePlayerNumUI(int32 PlayerNum);

	UFUNCTION(NetMulticast , Reliable)
	void MulticastHideGameUI();

	UFUNCTION(NetMulticast , Reliable)
	void MulticastUpdateRouletteUI(int32 Player , int32 Rule , int32 Result);

	UFUNCTION(NetMulticast , Reliable)
	void MulticastStartLuckyDraw();

	UFUNCTION(NetMulticast , Reliable)
	void MulticastPlayRouletteAnimation();

	UFUNCTION(NetMulticast , Reliable)
	void MulticastEndRounds();

	UFUNCTION(NetMulticast , Reliable)
	void MulticastShowOnlyNumPlayers();

	UFUNCTION(NetMulticast , Reliable)
	void MulticastDisableAllPhysics();
#pragma endregion

#pragma region 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TTSettings|Sound")
	class USoundBase* RouletteEndSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TTSettings|Sound")
	USoundAttenuation* LuckyDrawAttenuation;
	
	UFUNCTION(NetMulticast , Reliable)
	void MulticastPlayRouletteEndSound();
#pragma endregion 
private:
	int32 TotalRounds;
	int32 CurrentRound;
	FTimerHandle RoundTimerHandle;
	FTimerHandle LuckyDrawLoseTimerHandle;

	FVector OriginalLocation;
	FRotator OriginalRotation;
	bool bIsThrown;
};
