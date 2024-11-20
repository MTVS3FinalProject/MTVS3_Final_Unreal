// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_PuzzleWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/PuzzleManager.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"

class APuzzleManager;

void UHM_PuzzleWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	Btn_Exit->OnClicked.AddDynamic(this , &UHM_PuzzleWidget::OnClickedExitButton);
	
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
	// TextPlayerScores[0] = Text_Player1Score;
	// TextPlayerScores[1] = Text_Player2Score;
	// TextPlayerScores[2] = Text_Player3Score;
	// TextPlayerScores[3] = Text_Player4Score;
	// TextPlayerScores[4] = Text_Player5Score; 
	// TextPlayerScores[5] = Text_Player6Score;
	// TextPlayerScores[6] = Text_Player7Score;
	// TextPlayerScores[7] = Text_Player8Score;
	// TextPlayerScores[8] = Text_Player9Score;

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

// void UHM_PuzzleWidget::UpdatePlayerScores(const TArray<FPlayerScoreInfo>& PlayerScoresInfo)
// {
//     // 점수 정보 업데이트
// 	for (int32 Index = 0; Index < PlayerScoresInfo.Num(); Index++)
// 	{
// 		if(TextPlayerScores[Index] == nullptr)
// 		{
// 			UE_LOG(LogTemp, Log, TEXT("TextPlayerScores[%d] is null!!"), Index);
// 			return;
// 		}
// 		
// 		if (Index < 9 && TextPlayerScores[Index]) // 최대 9명까지 표시
// 		{
// 			UTTGameInstance* GI = Cast<UTTGameInstance>(PlayerScoresInfo[Index].Player->GetGameInstance());
// 			FString NickName;
// 			if (GI)
// 			{
// 				NickName = GI->GetNickname();
// 			}
// 			
// 			FString TimeString = PlayerScoresInfo[Index].Timestamp.ToString(TEXT("%M:%S"));
// 			FString ScoreText = FString::Printf(TEXT("#%d %s: %d / %s"), 
// 				Index + 1,  // 랭킹 표시
//                 //*PlayerScoresInfo[Index].Player->GetName(),
//                 *NickName,
//                 PlayerScoresInfo[Index].Score,
//                 *TimeString);
// 			
// 			//TextPlayerScores[Index]->SetText(FText::FromString(ScoreText));
// 			UTextBlock* TextBlock = TextPlayerScores[Index];
// 			if(TextBlock)
// 			{
// 				TextBlock->SetText(FText::FromString(ScoreText));
// 				UE_LOG(LogTemp , Log , TEXT("Successfully set text for player : %s") , *ScoreText);
// 			}
// 		}
// 	}
// }

void UHM_PuzzleWidget::SetTextPuzzleRank1Nickname(FString Nickname)
{
	Txt_Nickname_1->SetText(FText::FromString(Nickname));
	UE_LOG(LogTemp , Log , TEXT("UI Rank1 Nickname: %s"), *Nickname);
}

void UHM_PuzzleWidget::SetTextPuzzleRank1(UTexture2D* StickerImg, FString StickerRarity ,
	FString StickerName, FString StickerScript, FString TitleRarity, FString TitleName, FString TitleScript)
{
	Txt_StickerRarity_1->SetText(FText::FromString(StickerRarity));
	Txt_StickerName_1->SetText(FText::FromString(StickerName));
	Txt_StickerScript_1->SetText(FText::FromString(StickerScript));
	Txt_TitleRarity_1->SetText(FText::FromString(TitleRarity));
	Txt_TitleName_1->SetText(FText::FromString(TitleName));
	Txt_TitleScript_1->SetText(FText::FromString(TitleScript));
	
	Img_StickerImg_1->SetBrushFromTexture(StickerImg);
}

void UHM_PuzzleWidget::SetTextPuzzleRank2Nickname(FString Nickname)
{
	Txt_Nickname_2->SetText(FText::FromString(Nickname));
	UE_LOG(LogTemp , Log , TEXT("UI Rank2 Nickname: %s"), *Nickname);
}

void UHM_PuzzleWidget::SetTextPuzzleRank2(UTexture2D* StickerImg, FString StickerRarity,
	FString StickerName, FString StickerScript, FString TitleRarity, FString TitleName, FString TitleScript)
{
	Txt_StickerRarity_2->SetText(FText::FromString(StickerRarity));
	Txt_StickerName_2->SetText(FText::FromString(StickerName));
	Txt_StickerScript_2->SetText(FText::FromString(StickerScript));
	Txt_TitleRarity_2->SetText(FText::FromString(TitleRarity));
	Txt_TitleName_2->SetText(FText::FromString(TitleName));
	Txt_TitleScript_2->SetText(FText::FromString(TitleScript));
	
	Img_StickerImg_2->SetBrushFromTexture(StickerImg);
}

void UHM_PuzzleWidget::SetTextPuzzleRank3Nickname(FString Nickname)
{
	Txt_Nickname_3->SetText(FText::FromString(Nickname));
	UE_LOG(LogTemp , Log , TEXT("UI Rank3 Nickname: %s"), *Nickname);
}

void UHM_PuzzleWidget::SetTextPuzzleRank3(UTexture2D* StickerImg, FString StickerRarity,
	FString StickerName, FString StickerScript, FString TitleRarity, FString TitleName, FString TitleScript)
{
	Txt_StickerRarity_3->SetText(FText::FromString(StickerRarity));
	Txt_StickerName_3->SetText(FText::FromString(StickerName));
	Txt_StickerScript_3->SetText(FText::FromString(StickerScript));
	Txt_TitleRarity_3->SetText(FText::FromString(TitleRarity));
	Txt_TitleName_3->SetText(FText::FromString(TitleName));
	Txt_TitleScript_3->SetText(FText::FromString(TitleScript));
	
	Img_StickerImg_3->SetBrushFromTexture(StickerImg);
}

void UHM_PuzzleWidget::SetWidgetSwitcher(int32 num)
{
	WS_PuzzleSwitcher->SetActiveWidgetIndex(num);
}

void UHM_PuzzleWidget::SetTextVisibility(int32 Rank, ESlateVisibility InVisibility)
{
	switch (Rank)
	{
	case 1:
		if (VerticalBox_1)
			VerticalBox_1->SetVisibility(InVisibility);
		break;
	case 2:
		if (VerticalBox_2)
			VerticalBox_2->SetVisibility(InVisibility);
		break;
	case 3:
		if (VerticalBox_3)
			VerticalBox_3->SetVisibility(InVisibility);
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid rank for visibility setting: %d"), Rank);
		break;
	}
}

void UHM_PuzzleWidget::OnClickedExitButton()
{
	this->SetVisibility(ESlateVisibility::Hidden);
}
