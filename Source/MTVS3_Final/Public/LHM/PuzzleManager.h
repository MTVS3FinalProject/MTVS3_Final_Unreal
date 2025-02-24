// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HJ/TTGameInstance.h"
#include "PuzzleManager.generated.h"

USTRUCT()
struct FPlayerScoreInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString Player;

	UPROPERTY()
	int32 Score;

	UPROPERTY() // 동점일 경우 점수 도달 시점
	FDateTime Timestamp;

	// 기본 생성자
	FPlayerScoreInfo()
		: Player(TEXT("")), Score(0), Timestamp(0) {}

	// 생성자 오버로드: 초기화 시점 설정
	FPlayerScoreInfo(FString InPlayer, int32 InScore)
		: Player(InPlayer), Score(InScore), Timestamp(FDateTime::Now()) {}
};

// 복제될 플레이어 정보 구조체
USTRUCT(BlueprintType)
struct FPlayerRankInfo
{
	GENERATED_BODY()

	UPROPERTY()
	FString NickName;

	UPROPERTY()
	int32 Rank;

	UPROPERTY()
	int32 Score;

	UPROPERTY(BlueprintReadWrite)
	FString StickerImageUrl;

	UPROPERTY(BlueprintReadWrite)
	FString StickerRarity;

	UPROPERTY(BlueprintReadWrite)
	FString StickerName;

	UPROPERTY(BlueprintReadWrite)
	FString StickerScript;

	UPROPERTY(BlueprintReadWrite)
	FString TitleRarity;

	UPROPERTY(BlueprintReadWrite)
	FString TitleName;

	UPROPERTY(BlueprintReadWrite)
	FString TitleScript;
	

	FPlayerRankInfo() : NickName(TEXT("")), Rank(0), Score(0) {}
};

UENUM(BlueprintType)
enum class EPlayerRank : uint8
{
	First UMETA(DisplayName = "1st"),	// 1등
	Second UMETA(DisplayName = "2nd"),	// 2등
	Third UMETA(DisplayName = "3rd"),	// 3등
	None UMETA(DisplayName = "No Rank") // 순위 없음
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
	
	//EPlayerRank PlayerRank;
	//EPlayerRank GetPlayerRank() const { return PlayerRank; }
	//void SetPlayerRank(EPlayerRank NewRank) { PlayerRank = NewRank; }
	

#pragma region UI
	// UPROPERTY()
	// class UHM_PuzzleWidget* PuzzleUI;
	// void SetPuzzleUI(UHM_PuzzleWidget* InPuzzleUI);
#pragma endregion
	
	// 점수 관리
	UFUNCTION(BlueprintCallable, category = "Puzzle")
	void AddPiece(UStaticMeshComponent* Piece, int32 InitialScore);
	
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	int32 GetPieceScore(UStaticMeshComponent* Piece) const;

	// 플레이어에게 점수 부여
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	void AddScoreToPlayer(const FString& PlayerNickname, int32 Score);

public:
	TMap<UStaticMeshComponent*, int32> Pieces;

	// 각 플레이어의 점수 관리
	UPROPERTY()
	TMap<FString, int32> PlayerScores;

	// 플레이어 점수 정보를 저장할 배열
	TArray<FPlayerScoreInfo> PlayerScoresInfo;
	
	UPROPERTY()
	class UAudioComponent* HitAudioComp;
	UPROPERTY()
	class UAudioComponent* PuzzleEndingAudioComp;
	UFUNCTION()
	void PlayHit();
	UFUNCTION()
	void PlayPuzzleEnding();

#pragma region 퍼즐 종료 동기화 method
	
	// 퍼즐종료 & 랭킹업데이트
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	void SortAndUpdateRanking();

	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	void UpdatePlayerRankInfo();
	
	UFUNCTION(Client, Reliable)
	void Client_ReceiveRank(EPlayerRank Rank, const FString& Nickname);

	UFUNCTION(Server, Reliable)
	void Server_HandlePuzzleResult();
	
	// 복제될 배열
	UPROPERTY(Replicated)
	TArray<FPlayerRankInfo> ReplicatedRankInfo;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// KHJ
	// 퍼즐 게임 초기화 함수
	UFUNCTION()
	void ResetPuzzleGame();

	// 퍼즐 피스 클래스 참조
	UPROPERTY(EditDefaultsOnly, Category = "TTSettings|Puzzle")
	TSubclassOf<class AHM_PuzzlePiece> PuzzlePieceFactory;
#pragma endregion

private:
	bool bPuzzleCompleted = false;
};
