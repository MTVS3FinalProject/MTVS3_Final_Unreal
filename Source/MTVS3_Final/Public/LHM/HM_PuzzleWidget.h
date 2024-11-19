// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HM_PuzzleWidget.generated.h"

struct FPlayerScoreInfo;
/**
 * 
 */
UCLASS()
class MTVS3_FINAL_API UHM_PuzzleWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeConstruct() override;

	// TextPiece 포인터 배열
	class UTextBlock* TextPieces[9];
	
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Piece1;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Piece2;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Piece3;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Piece4;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Piece5;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Piece6;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Piece7;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Piece8;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Piece9;
	
	
	// TextPlayerScore 포인터 배열
	class UTextBlock* TextPlayerScores[9];
	
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Player1Score;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Player2Score;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Player3Score;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Player4Score;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Player5Score;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Player6Score;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Player7Score;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Player8Score;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Text_Player9Score;
	
	void SetTextPieceInfo(FString PieceName, int32 Score, int32 Index);
	
	//UFUNCTION()
	void UpdatePlayerScores(const TArray<FPlayerScoreInfo>& PlayerScoresInfo);
};
