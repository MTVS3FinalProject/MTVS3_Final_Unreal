// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/PuzzleManager.h"

#include "EngineUtils.h"
#include "HJ/TTPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor3.h"
#include "LHM/HM_PuzzleWidget.h"
#include "Net/UnrealNetwork.h"

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

void APuzzleManager::SetPuzzleUI(UHM_PuzzleWidget* InPuzzleUI)
{
	PuzzleUI = InPuzzleUI;
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

void APuzzleManager::AddScoreToPlayer(const FString& PlayerNickname, int32 Score)
{
	if (PlayerNickname.IsEmpty()) return;

	// 점수 추가 및 관리
	int32* CurrentScore = PlayerScores.Find(PlayerNickname);
	if (CurrentScore)
	{
		*CurrentScore += Score;
	}
	else
	{
		PlayerScores.Add(PlayerNickname, Score);
	}

	// PlayerScoresInfo 업데이트
	bool bPlayerFound = false;
	for(FPlayerScoreInfo& PlayerInfo : PlayerScoresInfo)
	{
		if(PlayerInfo.Player == PlayerNickname)
		{
			PlayerInfo.Score = PlayerScores[PlayerNickname];
			bPlayerFound = true;
			break;
		}
	}

	if(!bPlayerFound)
	{
		FPlayerScoreInfo NewPlayerInfo(PlayerNickname, PlayerScores[PlayerNickname]);
		PlayerScoresInfo.Add(NewPlayerInfo);
	}

	UE_LOG(LogTemp, Log, TEXT("Player %s new score: %d"), *PlayerNickname, PlayerScores[PlayerNickname]);
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, 
				FString::Printf(TEXT("Player %s new score: %d"),*PlayerNickname, PlayerScores[PlayerNickname]));
}

// 퍼즐이 종료됐을 때 호출되는 함수
void APuzzleManager::SortAndUpdateRanking()
{
	// 점수 기준으로 내림차순 정렬
	PlayerScoresInfo.Sort([](const FPlayerScoreInfo& A, const FPlayerScoreInfo& B) {
		if (A.Score == B.Score)
		{
			return A.Timestamp < B.Timestamp; // 동점일 경우 타임스탬프 기준
		}
		return A.Score > B.Score;  // 높은 점수가 우선
	});

	// 각 클라이언트에게 랭킹 정보를 전파
	UpdatePlayerRankInfo();

	TArray<FPlayerScoreInfo> TopPlayers;
	for (int32 i = 0; i < FMath::Min(3 , PlayerScoresInfo.Num()); i++)
	{
		TopPlayers.Add(PlayerScoresInfo[i]);
	}

	int32 TotalPlayers = TopPlayers.Num();

	// 각 클라이언트에 UI 정보 전파
	for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
	{
		ATTPlayer* TTPlayer = *It;
		if (TTPlayer)
		{
			// 자신의 순위를 전달
			for (int32 idx = 0; idx < TotalPlayers; idx++)
			{
				if (TTPlayer->GetNickname() == TopPlayers[idx].Player)
				{
					Client_ReceiveRank(static_cast<EPlayerRank>(idx + 1) , TTPlayer->GetNickname());
					//break;
				}
			}

			// UI 업데이트
			TTPlayer->Multicast_UpdatePuzzleRankAndVisibility(TopPlayers , TotalPlayers);
		}
	}
	
}

void APuzzleManager::UpdatePlayerRankInfo()
{
	ReplicatedRankInfo.Empty();

	for (int32 i = 0; i < FMath::Min(PlayerScoresInfo.Num(), 3); i++)
	{
		FString Player = PlayerScoresInfo[i].Player;
		if (!Player.IsEmpty())
		{
			// 월드에 있는 모든 ATTPlayer 순회
			for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
			{
				ATTPlayer* TTPlayer = *It;
				if (TTPlayer && TTPlayer->GetNickname() == Player)
				{
					FPlayerRankInfo RankInfo;
					RankInfo.NickName = TTPlayer->GetNickname();
					RankInfo.Rank = i + 1;
					RankInfo.Score = PlayerScoresInfo[i].Score;
					ReplicatedRankInfo.Add(RankInfo);

					UE_LOG(LogTemp, Log, TEXT("Rank Info: Rank %d, Nickname: %s, Score: %d"),
						RankInfo.Rank, *RankInfo.NickName, RankInfo.Score);
				}
			}
		}
	}
}

void APuzzleManager::Client_ReceiveRank_Implementation(EPlayerRank Rank, const FString& Nickname)
{
	if (Rank == EPlayerRank::None || static_cast<int32>(Rank) > 3) return;
	// 현재 클라이언트가 로컬 컨트롤러인지 확인
	//APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (PC && PC->IsLocalController())
	{
		// 월드에 있는 모든 ATTPlayer 순회
		for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
		{
			ATTPlayer* TTPlayer = *It;
			if (TTPlayer && TTPlayer->GetNickname() == Nickname)
			{
				FString AccessToken = TTPlayer->GetAccessToken();
				FString NickName = TTPlayer->GetNickname();

				UE_LOG(LogTemp , Log , TEXT("Client AccessToken: %s, Nickname: %s") , *AccessToken , *NickName);

				// 자신의 순위와 AccessToken으로 HTTP 요청 처리
				AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
					UGameplayStatics::GetActorOfClass(GetWorld() , AHM_HttpActor3::StaticClass()));
				if (!HttpActor3) return;

				HttpActor3->ReqPostPuzzleResultAndGetSticker(static_cast<int32>(Rank) ,static_cast<FString>(Nickname), AccessToken);
				
				break;
			}
		}
	}
}

void APuzzleManager::Server_HandlePuzzleResult_Implementation()
{
	for (APlayerController* PC : TActorRange<APlayerController>(GetWorld()))
	{
		if (ATTPlayer* Player = Cast<ATTPlayer>(PC->GetPawn()))
		{
			FString PlayerNickname = Player->GetNickname();
			
			// ReplicatedRankInfo에서 해당 플레이어가 상위 3위 안에 있는지 확인
			for (const FPlayerRankInfo& RankInfo : ReplicatedRankInfo)
			{
				if (RankInfo.NickName == PlayerNickname)
				{
					// Rank가 1, 2, 3 중 하나인지 확인
					if (RankInfo.Rank >= 1 && RankInfo.Rank <= 3)
					{
						UE_LOG(LogTemp, Log, TEXT("Updating UI for player: %s with rank: %d"), 
							   *PlayerNickname, RankInfo.Rank);
                    
						// UI 업데이트 호출
						Player->Client_UpdatePuzzleUI();
					}
					break;
				}
			}
		}
	}
}

void APuzzleManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APuzzleManager, ReplicatedRankInfo);
}
