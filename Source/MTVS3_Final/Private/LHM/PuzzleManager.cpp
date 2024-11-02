// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/PuzzleManager.h"

// Sets default values
APuzzleManager::APuzzleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APuzzleManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APuzzleManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APuzzleManager::AddPiece(UStaticMeshComponent* Piece, int32 InitialScore)
{
	if(Piece && !Pieces.Contains(Piece))
	{
		Pieces.Add(Piece, InitialScore);
	}
	UE_LOG(LogTemp, Log, TEXT("Piece %s Initial Score: %d"), *Piece->GetName(), InitialScore);
}

int32 APuzzleManager::GetPieceScore(UStaticMeshComponent* Piece) const
{
	const int32* Score = Pieces.Find(Piece);
	return Score ? *Score : 0;
}

// void APuzzleManager::SetPieceScore(UStaticMeshComponent* Piece, int32 NewScore)
// {
// 	if (Pieces.Contains(Piece))
// 	{
// 		Pieces[Piece] = NewScore;
// 	}
// }

void APuzzleManager::AddScoreToPlayer(AActor* Player, int32 Score)
{
	if (!Player) return;
	int32* CurrentScore = PlayerScores.Find(Player);
	if (CurrentScore)
	{
		*CurrentScore += Score;
	}
	else
	{
		PlayerScores.Add(Player, Score);
	}

	UE_LOG(LogTemp, Log, TEXT("Player %s new score: %d"), *Player->GetName(), PlayerScores[Player]);
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, 
				FString::Printf(TEXT("Player %s new score: %d"),*Player->GetName(), PlayerScores[Player]));
}

