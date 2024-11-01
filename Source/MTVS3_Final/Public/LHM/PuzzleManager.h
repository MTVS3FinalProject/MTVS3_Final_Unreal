// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleManager.generated.h"

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

	// 점수 관리
	UFUNCTION(BlueprintCallable, category = "Puzzle")
	void AddPiece(UStaticMeshComponent* Piece, int32 InitialScore);
	
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	int32 GetPieceScore(UStaticMeshComponent* Piece) const;

	//UFUNCTION(BlueprintCallable, Category = "Puzzle")
	//void SetPieceScore(UStaticMeshComponent* Piece, int32 NewScore);

	// 플레이어에게 점수 부여
	UFUNCTION(BlueprintCallable, Category = "Puzzle")
	void AddScoreToPlayer(AActor* Player, int32 Score);

private:
	TMap<UStaticMeshComponent*, int32> Pieces;

	// 각 플레이어의 점수 관리
	UPROPERTY()
	TMap<AActor*, int32> PlayerScores;
	
};
