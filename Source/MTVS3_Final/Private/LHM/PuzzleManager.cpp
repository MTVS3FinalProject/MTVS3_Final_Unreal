// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/PuzzleManager.h"

#include "HJ/TTGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "LHM/HM_HttpActor3.h"
#include "LHM/HM_PuzzlePiece.h"
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

	// Player의 Controller를 가져옴
	APawn* Pawn = Cast<APawn>(Player);
	if (!Pawn) return;
	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	if (!PC) return;
	UTTGameInstance* GI = Cast<UTTGameInstance>(PC->GetGameInstance());
	if (!GI) return;

	FString NickName = GI->GetNickname();

	UE_LOG(LogTemp, Log, TEXT("Player %s new score: %d"), *NickName, PlayerScores[Player]);
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Yellow, 
				FString::Printf(TEXT("Player %s new score: %d"),*NickName, PlayerScores[Player]));
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
	Multicast_UpdateUI(PlayerScoresInfo);

	// 각 클라이언트에 자신의 순위 전달
	for (int32 i = 0; i < FMath::Min(3, PlayerScoresInfo.Num()); i++)
	{
		if (AActor* Player = PlayerScoresInfo[i].Player)
		{
			APawn* Pawn = Cast<APawn>(Player);
			if (!Pawn) continue;

			APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
			if (!PC) continue;

			//SetPlayerRank(static_cast<EPlayerRank>(i + 1));
			// 클라이언트에서 자신의 순위를 처리하도록 호출
			Client_ReceiveRank(static_cast<EPlayerRank>(i + 1));
		}
	}
}

// 클라이언트에서 UI를 업데이트하기 위한 멀티캐스트 함수
void APuzzleManager::Multicast_UpdateUI_Implementation(const TArray<FPlayerScoreInfo>& SortedScores)
{
	for (int32 i = 0; i < FMath::Min(3, SortedScores.Num()); i++) // 3위까지만 처리
	{
		const FPlayerScoreInfo& PlayerInfo = SortedScores[i];

		// 플레이어의 컨트롤러를 가져와 자신의 닉네임을 가져오기
		if (AActor* Player = PlayerInfo.Player)
		{
			// Player의 Controller를 가져옴
			APawn* Pawn = Cast<APawn>(Player);
			if (!Pawn) return;

			APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
			if (!PC) return;

			// PlayerController의 GameInstance에서 데이터 가져오기
			UTTGameInstance* GI = Cast<UTTGameInstance>(PC->GetGameInstance());
			if (!GI) return;
			
			FString NickName = GI->GetNickname();

			// UI 업데이트
			UpdateUINickname(static_cast<EPlayerRank>(i + 1), NickName);
		}
	}

	if (!PuzzleUI) return;

	// 플레이어 수에 따른 UI 조정
	int32 PlayerCount = SortedScores.Num();
	if (PlayerCount == 1)
	{
		UE_LOG(LogTemp , Log , TEXT("PlayerCount == 1 UI 2,3 Hidden"));
		PuzzleUI->SetTextVisibility(2, ESlateVisibility::Hidden);
		PuzzleUI->SetTextVisibility(3, ESlateVisibility::Hidden);
	}
	else if (PlayerCount == 2)
	{
		UE_LOG(LogTemp , Log , TEXT("PlayerCount == 2 UI 3 Hidden"));
		PuzzleUI->SetTextVisibility(3, ESlateVisibility::Hidden);
	}
}

void APuzzleManager::UpdateUINickname(EPlayerRank Rank, const FString& NickName)
{
	// UI 업데이트
	
	switch (Rank)
	{
	case EPlayerRank::First:
		PuzzleUI->SetTextPuzzleRank1Nickname(NickName);
		UE_LOG(LogTemp , Log , TEXT("1# NickName: %s"), *NickName);
		break;
	case EPlayerRank::Second:
		PuzzleUI->SetTextPuzzleRank2Nickname(NickName);
		UE_LOG(LogTemp , Log , TEXT("2# NickName: %s"), *NickName);
		break;
	case EPlayerRank::Third:
		PuzzleUI->SetTextPuzzleRank3Nickname(NickName);
		UE_LOG(LogTemp , Log , TEXT("3# NickName: %s"), *NickName);
		break;
	default:
		return;
	}
}

void APuzzleManager::UpdatePlayerRankInfo()
{
	ReplicatedRankInfo.Empty();

	for (int32 i = 0; i < FMath::Min(PlayerScoresInfo.Num(), 3); i++)
	{
		if (AActor* Player = PlayerScoresInfo[i].Player)
		{
			// Player의 Controller를 가져옴
			APawn* Pawn = Cast<APawn>(Player);
			if (!Pawn) return;

			APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
			if (!PC) return;

			// PlayerController의 GameInstance에서 데이터 가져오기
			UTTGameInstance* GI = Cast<UTTGameInstance>(PC->GetGameInstance());
			if (GI)
			{
				FPlayerRankInfo RankInfo;
				RankInfo.NickName = GI->GetNickname();
				RankInfo.Rank = i + 1;
				RankInfo.Score = PlayerScoresInfo[i].Score;
				ReplicatedRankInfo.Add(RankInfo);

				UE_LOG(LogTemp, Log, TEXT("Rank Info: Rank %d, Nickname: %s, Score: %d"),
					RankInfo.Rank, *RankInfo.NickName, RankInfo.Score);
			}
		}
	}
}

void APuzzleManager::Client_ReceiveRank_Implementation(EPlayerRank Rank)
{
	if (Rank == EPlayerRank::None || static_cast<int32>(Rank) > 3) return;

	// 현재 클라이언트에서 자신의 Controller 및 GameInstance 가져오기
	APlayerController* LocalPC = GetWorld()->GetFirstPlayerController(); // LocalPlayer만 가져옴
	if (!LocalPC) return;
	
	if (LocalPC && LocalPC->IsLocalController())
	{
		UE_LOG(LogTemp, Log, TEXT("This is a local PlayerController."));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("This is not a local PlayerController."));
	}
	
	UTTGameInstance* LocalGI = Cast<UTTGameInstance>(LocalPC->GetGameInstance());
	if (!LocalGI) return;

	FString AccessToken = LocalGI->GetAccessToken();
	FString NickName = LocalGI->GetNickname();

	UE_LOG(LogTemp , Log , TEXT("Client AccessToken: %s, Nickname: %s"), *AccessToken, *NickName);
	
	// 자신의 순위와 AccessToken으로 HTTP 요청 처리
	AHM_HttpActor3* HttpActor3 = Cast<AHM_HttpActor3>(
		UGameplayStatics::GetActorOfClass(GetWorld(), AHM_HttpActor3::StaticClass()));
	if (!HttpActor3) return;

	HttpActor3->ReqPostPuzzleResultAndGetSticker(static_cast<int32>(Rank), AccessToken);
	
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		UE_LOG(LogTemp, Log, TEXT("PlayerController: %s"), *PC->GetName());
	}

	UTTGameInstance* GI = Cast<UTTGameInstance>(PC->GetGameInstance());
	if (GI)
	{
		UE_LOG(LogTemp, Log, TEXT("AccessToken: %s, Nickname: %s"), *GI->GetAccessToken(), *GI->GetNickname());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("GameInstance not found for this PlayerController."));
	}

	if (HasAuthority())
	{
		UE_LOG(LogTemp, Log, TEXT("This is the Server instance."));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("This is a Client instance."));
	}
}

void APuzzleManager::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(APuzzleManager, ReplicatedRankInfo);
}
