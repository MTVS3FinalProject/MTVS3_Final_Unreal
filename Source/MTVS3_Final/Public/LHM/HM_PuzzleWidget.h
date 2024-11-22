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
	UFUNCTION()
	void SetTextPieceInfo(FString PieceName, int32 Score, int32 Index);
	
	// // TextPlayerScore 포인터 배열
	// class UTextBlock* TextPlayerScores[9];
	// UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	// class UTextBlock* Text_Player1Score;
	// UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	// class UTextBlock* Text_Player2Score;
	// UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	// class UTextBlock* Text_Player3Score;
	// UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	// class UTextBlock* Text_Player4Score;
	// UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	// class UTextBlock* Text_Player5Score;
	// UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	// class UTextBlock* Text_Player6Score;
	// UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	// class UTextBlock* Text_Player7Score;
	// UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	// class UTextBlock* Text_Player8Score;
	// UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	// class UTextBlock* Text_Player9Score;
	//UFUNCTION()
	//void UpdatePlayerScores(const TArray<FPlayerScoreInfo>& PlayerScoresInfo);

	
	// 1#
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_Nickname_1;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_StickerImg_1;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_StickerRarity_1;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_StickerName_1;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_StickerScript_1;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_TitleRarity_1;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_TitleName_1;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_TitleScript_1;
	UFUNCTION()
    void SetTextPuzzleRank1Nickname(FString Nickname);
	UFUNCTION()
	void SetTextPuzzleRank1(UTexture2D* StickerImg, FString StickerRarity , FString StickerName, FString StickerScript, FString TitleRarity, FString TitleName, FString TitleScript);
	
	// 2#
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_Nickname_2;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_StickerImg_2;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_StickerRarity_2;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_StickerName_2;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_StickerScript_2;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_TitleRarity_2;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_TitleName_2;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_TitleScript_2;
	UFUNCTION()
    void SetTextPuzzleRank2Nickname(FString Nickname);
	UFUNCTION()
	void SetTextPuzzleRank2(UTexture2D* StickerImg, FString StickerRarity , FString StickerName, FString StickerScript, FString TitleRarity, FString TitleName, FString TitleScript);

	// 3#
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_Nickname_3;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UImage* Img_StickerImg_3;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_StickerRarity_3;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_StickerName_3;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_StickerScript_3;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_TitleRarity_3;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_TitleName_3;
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UTextBlock* Txt_TitleScript_3;
	UFUNCTION()
    void SetTextPuzzleRank3Nickname(FString Nickname);
    UFUNCTION()
	void SetTextPuzzleRank3(UTexture2D* StickerImg, FString StickerRarity , FString StickerName, FString StickerScript, FString TitleRarity, FString TitleName, FString TitleScript);

	// 0: 피스 스코어, 1: 퍼즐 결과
	UPROPERTY(VisibleAnywhere , meta=(BindWidget))
	class UWidgetSwitcher* WS_PuzzleSwitcher;
	
	UFUNCTION()
	void SetWidgetSwitcher(int32 num);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UCanvasPanel* Canvas_1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UCanvasPanel* Canvas_2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UCanvasPanel* Canvas_3;
	UFUNCTION()
	void SetTextVisibility(int32 Rank, ESlateVisibility InVisibility);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	class UButton* Btn_Exit;
	UFUNCTION()
	void OnClickedExitButton();
};
