// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_PuzzleWidget.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/PuzzleManager.h"
#include "Slate/SGameLayerManager.h"

class APuzzleManager;

void UHM_PuzzleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// TextPiece 포인터 배열로 초기화
	TextPieces[0] = Text_Piece1;
	TextPieces[1] = Text_Piece2;
	TextPieces[2] = Text_Piece3;
	TextPieces[3] = Text_Piece4;
	TextPieces[4] = Text_Piece5; 
	TextPieces[5] = Text_Piece6;
	TextPieces[6] = Text_Piece7;
	TextPieces[7] = Text_Piece8;
	TextPieces[8] = Text_Piece9;

	// TextPlayerScore 포인터 배열로 초기화
	TextPlayerScores[0] = Text_Player1Score;
	TextPlayerScores[1] = Text_Player2Score;
	TextPlayerScores[2] = Text_Player3Score;
	TextPlayerScores[3] = Text_Player4Score;
	TextPlayerScores[4] = Text_Player5Score; 
	TextPlayerScores[5] = Text_Player6Score;
	TextPlayerScores[6] = Text_Player7Score;
	TextPlayerScores[7] = Text_Player8Score;
	TextPlayerScores[8] = Text_Player9Score;
	
	APuzzleManager* Manager = Cast<APuzzleManager>(UGameplayStatics::GetActorOfClass(GetWorld(), APuzzleManager::StaticClass()));
	if (Manager)
	{
		int32 Index = 0;
		for(const auto& PieceEntry : Manager->Pieces)
		{
			if(Index >= 9) break;

			UStaticMeshComponent* PieceComp = PieceEntry.Key;
			int32 Score = PieceEntry.Value;

			if(PieceComp)
			{
				FString PieceName = PieceComp->GetName();
				SetTextPieceInfo(PieceName, Score, Index);
			}
			Index++;
		}
	}
}

void UHM_PuzzleWidget::SetTextPieceInfo(FString PieceName, int32 Score, int32 Index)
{
	if (Index >= 0 && Index < 9)
	{
		UTextBlock* TextBlock = TextPieces[Index];
		if (TextBlock)
		{
			FString PieceScore = FString::Printf(TEXT("%s: %d"), *PieceName, Score);
			TextBlock->SetText(FText::FromString(PieceScore));
		}
	}
}

void UHM_PuzzleWidget::UpdatePlayerScores(const TArray<FPlayerScoreInfo>& PlayerScoresInfo)
{
	UE_LOG(LogTemp, Display, TEXT("UHM_PuzzleWidget::UpdatePlayerScores"));
	for (int32 Index = 0; Index < PlayerScoresInfo.Num(); Index++)
	{
		if (Index < 9) // 최대 9명까지 표시
		{
			FString ScoreText = FString::Printf(TEXT("%s: %d"), *PlayerScoresInfo[Index].Player->GetName(), PlayerScoresInfo[Index].Score);
			if (TextPlayerScores[Index])
			{
				TextPlayerScores[Index]->SetText(FText::FromString(ScoreText));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("TextPlayerScores[%d] is null!"), Index);
			}
		}
	}
}
