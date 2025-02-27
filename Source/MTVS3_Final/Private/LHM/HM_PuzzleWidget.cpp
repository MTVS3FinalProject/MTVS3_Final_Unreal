// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_PuzzleWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/PuzzleManager.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
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

void UHM_PuzzleWidget::SetTextPuzzleRankNickname(int32 Rank, const FString& Nickname)
{
	switch (Rank)
	{
	case 1:
		if (Txt_Nickname_1)
		{
			Txt_Nickname_1->SetText(FText::FromString(Nickname));
			UE_LOG(LogTemp , Log , TEXT("UI Rank1 Nickname: %s"), *Nickname);
		}
		break;
	case 2:
		if (Txt_Nickname_2)
		{
			Txt_Nickname_2->SetText(FText::FromString(Nickname));
			UE_LOG(LogTemp , Log , TEXT("UI Rank2 Nickname: %s"), *Nickname);
		}
		break;
	case 3:
		if (Txt_Nickname_3)
		{
			Txt_Nickname_3->SetText(FText::FromString(Nickname));
			UE_LOG(LogTemp , Log , TEXT("UI Rank3 Nickname: %s"), *Nickname);
		}
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Invalid rank provided to SetTextPuzzleRankNickname: %d"), Rank);
		break;
	}
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
		if (Canvas_1)
		{
			Canvas_1->SetVisibility(InVisibility);
			Canvas_1->InvalidateLayoutAndVolatility(); // 레이아웃 강제 업데이트
		}
		break;
	case 2:
		if (Canvas_2)
		{
			Canvas_2->SetVisibility(InVisibility);
			Canvas_2->InvalidateLayoutAndVolatility(); // 레이아웃 강제 업데이트
		}
		break;
	case 3:
		if (Canvas_3)
		{
			Canvas_3->SetVisibility(InVisibility);
			Canvas_3->InvalidateLayoutAndVolatility(); // 레이아웃 강제 업데이트
		}
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
