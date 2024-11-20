// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleManager.generated.h"

USTRUCT()
struct FPlayerScoreInfo
{
	GENERATED_BODY()

	UPROPERTY()
	AActor* Player;

	UPROPERTY()
	int32 Score;

	UPROPERTY() // 동점일 경우 점수 도달 시점
	FDateTime Timestamp;

	// 기본 생성자
	FPlayerScoreInfo()
		: Player(nullptr), Score(0), Timestamp(0) {}

	// 생성자 오버로드: 초기화 시점 설정
	FPlayerScoreInfo(AActor* InPlayer, int32 InScore)
		: Player(InPlayer), Score(InScore), Timestamp(FDateTime::Now()) {}
};

UCLASS()
class MTVS3_FINAL_API APuzzleManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APuzzleManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

#pragma region UI
	UPROPERTY(EditAnywhere, Category = "Defalut|UI")
	TSubclassOf<class UHM_PuzzleWidget> PuzzleUIFactory;
	UPROPERTY()
	class UHM_PuzzleWidget* PuzzleUI;
#pragma endregion
	
	// 점수 관리
	UFUNCTION(BlueprintCallable, category = "Puzzle")
	void AddPiece(UStaticMeshComponent* Piece, int32 InitialScore);
	
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	int32 GetPieceScore(UStaticMeshComponent* Piece) const;

	// 플레이어에게 점수 부여
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	void AddScoreToPlayer(AActor* Player, int32 Score);

	// 퍼즐종료 & 랭킹업데이트
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	void SortAndUpdateRanking();

	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	void ProcessPlayerRanking(int32 Rank, const FString& NickName, const FString& AccessToken, AHM_HttpActor3* HttpActor3);

public:
	TMap<UStaticMeshComponent*, int32> Pieces;

	// 각 플레이어의 점수 관리
	UPROPERTY()
	TMap<AActor*, int32> PlayerScores;

	// 플레이어 점수 정보를 저장할 배열
	TArray<FPlayerScoreInfo> PlayerScoresInfo;

private:
	bool bPuzzleCompleted = false;
};
