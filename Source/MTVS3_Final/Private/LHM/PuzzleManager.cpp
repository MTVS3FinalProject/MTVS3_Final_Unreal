// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/PuzzleManager.h"

#include "HJ/TTGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor3.h"
#include "LHM/HM_PuzzlePiece.h"
#include "LHM/HM_PuzzleWidget.h"

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

	PuzzleUI = CastChecked<UHM_PuzzleWidget>(CreateWidget(GetWorld() , PuzzleUIFactory));

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
	UE_LOG(LogTemp, Log, TEXT("%s Initial Score: %d"), *Piece->GetName(), InitialScore);
}

int32 APuzzleManager::GetPieceScore(UStaticMeshComponent* Piece) const
{
	const int32* Score = Pieces.Find(Piece);
	return Score ? *Score : 0;
}

void APuzzleManager::AddScoreToPlayer(AActor* Player, int32 Score)
{
	if (!Player) return;

	// 점수 추가 및 관리
	int32* CurrentScore = PlayerScores.Find(Player);
	if (CurrentScore)
	{
		*CurrentScore += Score;
	}
	else
	{
		PlayerScores.Add(Player, Score);
	}

	// PlayerScoresInfo 업데이트
	bool bPlayerFound = false;
	for(FPlayerScoreInfo& PlayerInfo : PlayerScoresInfo)
	{
		if(PlayerInfo.Player == Player)
		{
			PlayerInfo.Score = PlayerScores[Player];
			bPlayerFound = true;
			break;
		}
	}

	if(!bPlayerFound)
	{
		FPlayerScoreInfo NewPlayerInfo(Player, PlayerScores[Player]);
		PlayerScoresInfo.Add(NewPlayerInfo);
	}

	UTTGameInstance* GI = Cast<UTTGameInstance>(Player->GetGameInstance());
	FString NickName;
	if (GI)
	{
		NickName = GI->GetNickname();
	}

	UE_LOG(LogTemp, Log, TEXT("Player %s new score: %d"), *NickName, PlayerScores[Player]);
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, 
				FString::Printf(TEXT("Player %s new score: %d"),*NickName, PlayerScores[Player]));
	
	// UE_LOG(LogTemp, Log, TEXT("Player %s new score: %d"), *Player->GetName(), PlayerScores[Player]);
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, 
	// 			FString::Printf(TEXT("Player %s new score: %d"),*Player->GetName(), PlayerScores[Player]));
	
	// 랭킹 산정 및 UI 업데이트
	//SortAndUpdateRanking();
}

void APuzzleManager::SortAndUpdateRanking()
{
	UE_LOG(LogTemp, Log, TEXT("Game Over"));
	
	// 점수 기준으로 내림차순 정렬
	PlayerScoresInfo.Sort([](const FPlayerScoreInfo& A, const FPlayerScoreInfo& B) {
		if (A.Score == B.Score)
		{
			// 점수가 같다면 Timestamp로 정렬 (먼저 도달한 순서대로)
			return A.Timestamp < B.Timestamp;
		}
		return A.Score > B.Score;  // 높은 점수가 우선 ('>' 는 내림차순, '<' 는 오름차순)
	});
	
        AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
            UGameplayStatics::GetActorOfClass(GetWorld(), AHM_HttpActor3::StaticClass()));
        
        if (!HttpActor3)
        {
            UE_LOG(LogTemp, Warning, TEXT("HttpActor3 is null!"));
            return;
        }
	
	// 순위별 작업 수행
	for (int32 i = 0; i < PlayerScoresInfo.Num(); i++)
	{
		UTTGameInstance* GI = Cast<UTTGameInstance>(PlayerScoresInfo[i].Player->GetGameInstance());
		if (!GI)
		{
			UE_LOG(LogTemp, Warning, TEXT("GameInstance is null for player %d!"), i + 1);
			continue;
		}
		
		// 디버그 로그로 정렬된 점수 출력
		FString NickName = GI->GetNickname();
		FString TimeString = PlayerScoresInfo[i].Timestamp.ToString(TEXT("%M:%S"));
		UE_LOG(LogTemp , Log , TEXT("Rank %d - Player: %s, Score: %d, Time %s") ,
		       i + 1 ,
		       //*PlayerScoresInfo[i].Player->GetName(),
		       *NickName ,
		       PlayerScoresInfo[i].Score ,
		       *TimeString)

		// 순위별 UI 및 서버 요청 처리
		ProcessPlayerRanking(i + 1, NickName, GI->GetAccessToken(), HttpActor3);
		
		// 순위가 3위까지 끝나면 종료
		if (i + 1 == 3)
			break;
	}
}

void APuzzleManager::ProcessPlayerRanking(int32 Rank, const FString& NickName, const FString& AccessToken,
	AHM_HttpActor3* HttpActor3)
{
	UE_LOG(LogTemp, Log, TEXT("ProcessPlayerRanking"));
	
	if (!PuzzleUI)
	{
		UE_LOG(LogTemp, Warning, TEXT("PuzzleUI is null!"));
		return;
	}

	// UI 업데이트
	switch (Rank)
	{
	case 1:
		PuzzleUI->SetTextPuzzleRank1Nickname(NickName);
		UE_LOG(LogTemp , Log , TEXT("PuzzleUI->SetTextPuzzleRank1Nickname"));
		break;
	case 2:
		PuzzleUI->SetTextPuzzleRank2Nickname(NickName);
		UE_LOG(LogTemp , Log , TEXT("PuzzleUI->SetTextPuzzleRank2Nickname"));
		break;
	case 3:
		PuzzleUI->SetTextPuzzleRank3Nickname(NickName);
		UE_LOG(LogTemp , Log , TEXT("PuzzleUI->SetTextPuzzleRank3Nickname"));
		break;
	default:
		return;
	}

	// 서버 요청
	if (HttpActor3)
	{
		HttpActor3->ReqPostPuzzleResultAndGetSticker(Rank, AccessToken);
		UE_LOG(LogTemp , Log , TEXT("HttpActor3->ReqPostPuzzleResultAndGetSticker"));
	}
}

