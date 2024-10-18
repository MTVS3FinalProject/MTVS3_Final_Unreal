// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
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
/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API ATTLuckyDrawGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
    virtual void BeginPlay() override;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RouletteTestMode|TTSettings")
    bool bIsRouletteTestMode;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RouletteTestMode|TTSettings")
    int32 NumPlayers = 30;  // 기본값 30명

    // 다음 라운드로 넘어가기까지의(탈락자 애니메이션) 시간
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RouletteTestMode|TTSettings")
    float EliminationEffectDuration = 3.0f;

    // 룰렛 딜레이
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RouletteTestMode|TTSettings")
    float RouletteDelay = 1.5f;

    TArray<TArray<FSeat>> Seats;
    TArray<int32> RemainingPlayers;

private:

    // 룰렛 타이머
    FTimerHandle RouletteTimer;
    int Round = 0;

    // 룰렛 관련 함수
    void StartRound();
    void SelectRouletteOptions();
    void ApplyRouletteOutcome(int32 Player , const FString& RowOrColRule , const FString& PassOrFail);
    void UpdateSeats();

    void GetAffectedPlayers(int32 SelectedPlayer , const FString& RowOrColRule , TArray<int32>& AffectedPlayers);

    // 유틸리티 함수
    void ShuffleSeats();
    void PrintSeats();
    bool IsGameOver() const;
    bool CheckSingleRowOrColRemaining();
    void GetPlayerPosition(int32 Player , int32& OutRow , int32& OutCol);
};
