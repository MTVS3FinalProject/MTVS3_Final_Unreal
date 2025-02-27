// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/PuzzleManager.h"

#include "EngineUtils.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "HJ/TTPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor3.h"
#include "LHM/HM_PuzzleBoard.h"
#include "LHM/HM_PuzzlePiece.h"
#include "LHM/HM_PuzzleWidget.h"
#include "Net/UnrealNetwork.h"

// Sets default values
APuzzleManager::APuzzleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HitAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("HitAudioComp"));
	if (HitAudioComp)
	{
		HitAudioComp->bAutoActivate = false; // 자동 재생 비활성화

		static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(TEXT("/Game/LHM/Sound/PUZZLE_SUCCES"));
		if (SoundAsset.Succeeded() && HitAudioComp)
		{
			HitAudioComp->SetupAttachment(RootComponent);
			HitAudioComp->SetSound(SoundAsset.Object);
		}
	}

	PuzzleEndingAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("PuzzleEndingAudioComp"));
	if (PuzzleEndingAudioComp)
	{
		PuzzleEndingAudioComp->bAutoActivate = false; // 자동 재생 비활성화

		static ConstructorHelpers::FObjectFinder<USoundBase> SoundAsset(TEXT("/Game/LHM/Sound/PUZZLE_END"));
		if (SoundAsset.Succeeded() && PuzzleEndingAudioComp)
		{
			PuzzleEndingAudioComp->SetupAttachment(RootComponent);
			PuzzleEndingAudioComp->SetSound(SoundAsset.Object);
		}
	}
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

// void APuzzleManager::SetPuzzleUI(UHM_PuzzleWidget* InPuzzleUI)
// {
// 	PuzzleUI = InPuzzleUI;
// }

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

void APuzzleManager::PlayHit()
{
	// 히트 사운드 재생
	if (HitAudioComp && HitAudioComp->Sound)
	{
		HitAudioComp->Play();
	}
}

void APuzzleManager::PlayPuzzleEnding()
{
	// 종료 사운드 재생
	if (PuzzleEndingAudioComp && PuzzleEndingAudioComp->Sound)
	{
		PuzzleEndingAudioComp->Play();
	}
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
					Client_ReceiveRank(static_cast<EPlayerRank>(idx) , TTPlayer->GetNickname());
					//break;
				}
			}

			// UI 업데이트
			//TTPlayer->Multicast_UpdatePuzzleRankAndVisibility(TopPlayers , TotalPlayers);
		}
	}
	
	// 5초 후에 퍼즐 게임 초기화
	FTimerHandle ResetTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		ResetTimerHandle,
		this,
		&APuzzleManager::ResetPuzzleGame,
		5.0f,
		false
	);
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
			if (TTPlayer)
			{
				UE_LOG(LogTemp, Log, TEXT("Iterating Player: %s"), *TTPlayer->GetNickname());

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

void APuzzleManager::ResetPuzzleGame()
{
    if (!HasAuthority()) return;

    // 1. 점수 초기화
    PlayerScores.Empty();
    PlayerScoresInfo.Empty();
    ReplicatedRankInfo.Empty();
	Pieces.Empty();

	// 2. 퍼즐 보드 초기화
	AHM_PuzzleBoard* PuzzleBoard = Cast<AHM_PuzzleBoard>(UGameplayStatics::GetActorOfClass(GetWorld(), AHM_PuzzleBoard::StaticClass()));
	if (PuzzleBoard)
	{
		// 보드 상태 배열 초기화
		PuzzleBoard->BoardAreaVisibility.Init(false, 9);
		PuzzleBoard->DestroyedPieceTags.Empty();

		// 보드 영역 가시성 초기화
		for (int32 i = 0; i < PuzzleBoard->BoardAreas.Num(); i++)
		{
			PuzzleBoard->ServerSetBoardAreaVisibility(i, false);
            
			// 나이아가라 이펙트 비활성화
			if (PuzzleBoard->NiagaraEffects.IsValidIndex(i) && PuzzleBoard->NiagaraEffects[i])
			{
				PuzzleBoard->NiagaraEffects[i]->Deactivate();
			}
		}
	}

    // 3. 사운드 컴포넌트 초기화
    if (HitAudioComp)
    {
        HitAudioComp->Stop();
    }
    if (PuzzleEndingAudioComp)
    {
        PuzzleEndingAudioComp->Stop();
    }

    // 4. 퍼즐 피스 재생성
    if (!PuzzlePieceFactory)
    {
        UE_LOG(LogTemp, Warning, TEXT("PuzzlePieceFactory is not set in PuzzleManager"));
        return;
    }

    // 기존 피스 제거
    TArray<AActor*> ExistingPieces;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHM_PuzzlePiece::StaticClass(), ExistingPieces);
    
    for (AActor* Actor : ExistingPieces)
    {
        if (AHM_PuzzlePiece* ExistingPiece = Cast<AHM_PuzzlePiece>(Actor))
        {
            // Transform 업데이트 타이머 제거
            GetWorld()->GetTimerManager().ClearTimer(ExistingPiece->TransformUpdateTimer);
            
            // 소유권 관련 맵 초기화
            ExistingPiece->ComponentOwners.Empty();
            ExistingPiece->LastOwners.Empty();
        }
        Actor->Destroy();
    }

    // 새로운 피스 스폰
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    // 퍼즐 피스 스폰 위치 설정
    FVector SpawnLocation(-3.0f, -2074.0f, 341.0f);
    
    AHM_PuzzlePiece* NewPiece = GetWorld()->SpawnActor<AHM_PuzzlePiece>(
        PuzzlePieceFactory,
        SpawnLocation,
        FRotator::ZeroRotator,
        SpawnParams
    );

    if (NewPiece)
    {
        NewPiece->InitializeRandomSetting();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to spawn new puzzle piece"));
    }
}
