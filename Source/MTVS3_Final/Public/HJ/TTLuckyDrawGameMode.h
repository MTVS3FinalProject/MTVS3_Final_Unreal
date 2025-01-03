﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TTLuckyDrawGameMode.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLuckyDraw , Log , All);  // 로깅 카테고리 선언

USTRUCT(BlueprintType)
struct FSeat
{
    GENERATED_BODY()

    UPROPERTY()
    int32 PlayerNumber = -1;  // -1은 빈 좌석을 의미

    bool IsOccupied() const { return PlayerNumber != -1; }
};


UENUM(BlueprintType)
enum class ERouletteRule : uint8
{
    SameColumnOnly = 0, // 과(와) 같은 열만
    SameRowOnly = 1, // 과(와) 같은 행만
    ExcludeSameColumn = 2, // 과(와) 같은 열 제외
    ExcludeSameRow = 3, // 과(와) 같은 행 제외
    OnlySelected = 4, // 만
};

UENUM(BlueprintType)
enum class ERouletteResult : uint8
{
    Pass = 0,
    Eliminate = 1,
};

USTRUCT(BlueprintType)
struct FRouletteInfo
{
    GENERATED_BODY()

    FRouletteInfo()
        : Player(-1)  // Player 필드를 -1로 초기화
        , Rule(ERouletteRule::OnlySelected)  // 기본 룰을 설정
        , Result(ERouletteResult::Pass)  // 기본 결과를 통과로 설정
    {
    }

    UPROPERTY()
    int32 Player;

    UPROPERTY()
    ERouletteRule Rule;

    UPROPERTY()
    ERouletteResult Result;
};

/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API ATTLuckyDrawGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
    ATTLuckyDrawGameMode();

    void PostLogin(APlayerController* NewPlayer);

    virtual void BeginPlay() override;

    void StartLuckyDraw(int32 PlayerNum);

    const TArray<TArray<FSeat>>& GetShuffledSeats() const;
    TArray<TArray<FSeat>> SavedSeats;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "TTSettings|Custom")
    bool bIsRouletteTestMode;

    UPROPERTY(VisibleInstanceOnly)
    int32 RouletteTestNumPlayers = 30;  // 기본값 30명

    TArray<TArray<FSeat>> Seats;
    TArray<int32> RemainingPlayers;

    // 추첨 테스트 라운드 간 딜레이
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TTSettings|Custom")
    float EliminationEffectDuration = 0.1f;

    // 총 라운드 수
    int Round = 0;

    // 각 라운드의 룰렛 정보를 저장하는 배열
    TArray<FRouletteInfo> RouletteInfosPerRound;

    // 라운드별 탈락자 기록을 저장하는 배열
    TArray<TArray<int32>> EliminatedPlayersPerRound;

    // 각 라운드에서 탈락한 플레이어를 저장하는 배열
    TArray<int32> RoundEliminatedPlayers;

    const FRouletteInfo& GetRouletteInfoForRound(int32 RoundIndex) const;

    void ResetLuckyDraw();
    
private:

    FTimerHandle RouletteTimer;

    // 룰렛 관련 함수
    void StartRound();
    void SelectRouletteOptions();
    void PrintSimulatedSeats(const TArray<TArray<FSeat>>& SimulatedSeats);
    void UpdateSimulatedSeats(TArray<TArray<FSeat>>& SimulatedSeats , const TArray<int32>& SimulatedRemainingPlayers);
    void ApplyRouletteOutcome(int32 Player , ERouletteRule Rule , ERouletteResult Outcome);
    void UpdateSeats();

    void GetAffectedPlayers(int32 SelectedPlayer , ERouletteRule Rule , TArray<int32>& AffectedPlayers);

    // 유틸리티 함수
    void ShuffleSeats();
    void PrintSeats();
    bool IsGameOver() const;
    bool CheckSingleRowOrColRemaining();
    void GetPlayerPosition(int32 Player , int32& OutRow , int32& OutCol);
};
