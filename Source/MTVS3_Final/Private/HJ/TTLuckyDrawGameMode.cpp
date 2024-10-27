#include "HJ/TTLuckyDrawGameMode.h"
#include "random"
#include "HJ/TTLuckyDrawGameState.h"
#include "HJ/TTPlayer.h"
#include "JMH/MH_GameWidget.h"

// LogLuckyDraw 카테고리 정의
DEFINE_LOG_CATEGORY(LogLuckyDraw);

ATTLuckyDrawGameMode::ATTLuckyDrawGameMode()
{
	// 배열 초기화
	EliminatedPlayersPerRound.Empty();
	RoundEliminatedPlayers.Empty();
	RouletteInfosPerRound.Empty();
}

void ATTLuckyDrawGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ATTLuckyDrawGameState* LocalGameState = GetGameState<ATTLuckyDrawGameState>();
	if ( LocalGameState )
	{
		LocalGameState->AssignSeatNumber(NewPlayer->PlayerState);
		LocalGameState->MulticastUpdatePlayerNumUI(LocalGameState->NewSeatNumber);
	}
}

void ATTLuckyDrawGameMode::BeginPlay()
{
	Super::BeginPlay();

	ATTLuckyDrawGameState* GS = GetGameState<ATTLuckyDrawGameState>();
	if (GS)
	{
		// 델리게이트 바인딩
		GS->OnRequestMovePlayersToChairs.AddDynamic(GS, &ATTLuckyDrawGameState::MovePlayersToChairs);
	}
}

void ATTLuckyDrawGameMode::StartLuckyDraw(int32 PlayerNum)
{
	RouletteTestNumPlayers = PlayerNum;
	// 인원 수를 출력
	if ( GEngine )
	{
		GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Yellow , FString::Printf(TEXT("설정된 인원 수: %d") , RouletteTestNumPlayers));
	}

	// 3*10 좌석 배열 초기화 (세로 3줄, 가로 10줄)
	Seats.SetNum(3);  // 열(세로) 수 3
	for ( int i = 0; i < 3; ++i )
	{
		Seats[i].SetNum(10);  // 각 열에 10개의 좌석
	}

	// 플레이어 번호 부여 및 랜덤 배치
	RemainingPlayers.Reserve(RouletteTestNumPlayers);
	for ( int32 i = 0; i < RouletteTestNumPlayers; ++i )
	{
		RemainingPlayers.Add(i + 1);  // 플레이어 번호는 1부터 시작
	}

	ShuffleSeats();

	// 첫 라운드 시작
	StartRound();
}

const TArray<TArray<FSeat>>& ATTLuckyDrawGameMode::GetShuffledSeats() const
{
	return SavedSeats;
}

const FRouletteInfo& ATTLuckyDrawGameMode::GetRouletteInfoForRound(int32 RoundIndex) const
{
	const FRouletteInfo& Info = RouletteInfosPerRound[RoundIndex];
	UE_LOG(LogLuckyDraw, Log, TEXT("현재 라운드: %d, Player: %d, Rule: %d, Result: %d"), 
		RoundIndex, Info.Player, static_cast<int32>(Info.Rule), static_cast<int32>(Info.Result));
	// 요청된 라운드의 룰렛 정보를 반환. 범위를 초과하는 경우 마지막 값을 반환
	return RouletteInfosPerRound.IsValidIndex(RoundIndex) ? RouletteInfosPerRound[RoundIndex] : RouletteInfosPerRound.Last();
}

// 라운드를 시작하는 함수
void ATTLuckyDrawGameMode::StartRound()
{
	if ( IsGameOver() )
	{
		UE_LOG(LogLuckyDraw , Log , TEXT("___________"));
		UE_LOG(LogLuckyDraw , Log , TEXT("총 라운드 수: %d") , Round);
		UE_LOG(LogLuckyDraw , Log , TEXT("최종 승자: %d") , RemainingPlayers[0]);

		// 게임 종료 시 라운드별 탈락자 리스트 출력
		if ( bIsRouletteTestMode ) UE_LOG(LogLuckyDraw , Log , TEXT("게임 종료! 라운드별 탈락자 및 룰렛 정보 출력:"));
		for ( int32 i = 0; i < EliminatedPlayersPerRound.Num(); ++i )
		{
			if ( RouletteInfosPerRound.Num() > i )
			{
				// 룰렛 정보 출력
				const FRouletteInfo& Info = RouletteInfosPerRound[i];
				UE_LOG(LogLuckyDraw , Log , TEXT("라운드%d 룰렛 정보: %d %d %d") , i + 1 , Info.Player , static_cast<int32>(Info.Rule) , static_cast<int32>(Info.Result));
			}

			// 탈락자 정보 출력
			FString EliminatedPlayersString;
			for ( int32 Player : EliminatedPlayersPerRound[i] )
			{
				EliminatedPlayersString += FString::Printf(TEXT("%d ") , Player);
			}

			if ( !EliminatedPlayersString.IsEmpty() )
			{
				UE_LOG(LogLuckyDraw , Log , TEXT("라운드%d 탈락자: %s") , i + 1 , *EliminatedPlayersString);
			}
		}
		// 종료
		UE_LOG(LogLuckyDraw , Log , TEXT("___________"));
		ATTLuckyDrawGameState* GS = GetGameState<ATTLuckyDrawGameState>();
		if (GS) GS->OnRequestMovePlayersToChairs.Broadcast();
		return;
	}

	Round++;
	UE_LOG(LogLuckyDraw , Log , TEXT("라운드 수: %d") , Round);
	PrintSeats();

	//SelectRouletteOptions();
	GetWorld()->GetTimerManager().SetTimer(RouletteTimer , this , &ATTLuckyDrawGameMode::SelectRouletteOptions , EliminationEffectDuration , false);
}

// 룰렛 옵션을 선택하는 함수
void ATTLuckyDrawGameMode::SelectRouletteOptions()
{
	if ( RemainingPlayers.Num() == 0 )
	{
		if ( bIsRouletteTestMode ) UE_LOG(LogLuckyDraw , Error , TEXT("남은 플레이어가 없습니다. 룰렛을 돌릴 수 없습니다."));
		return;
	}

	bool IsValidResult = false;
	int32 SelectedPlayer = -1;
	ERouletteRule SelectedRule = ERouletteRule::SameColumnOnly;
	ERouletteResult RouletteOutcome = ERouletteResult::Pass;

	// 시뮬레이션용 좌석 배열 생성 (실제 좌석과 별도로 관리)
	TArray<TArray<FSeat>> SimulatedSeats = Seats;

	while ( !IsValidResult )
	{
		int32 PlayerIndex = FMath::RandRange(0 , RemainingPlayers.Num() - 1);
		SelectedPlayer = RemainingPlayers[PlayerIndex];

		int32 RuleIndex = FMath::RandRange(0 , 4);  // 룰렛 규칙
		SelectedRule = static_cast<ERouletteRule>(RuleIndex);

		int32 OutcomeIndex = FMath::RandRange(0 , 1);  // 통과 or 탈락
		RouletteOutcome = static_cast<ERouletteResult>(OutcomeIndex);

		// 룰렛2 규칙에 따른 영향을 받는 플레이어 계산
		TArray<int32> AffectedPlayers;
		GetAffectedPlayers(SelectedPlayer , SelectedRule , AffectedPlayers);

		// 한 행 또는 한 열만 남았을 때 전멸 방지 처리
		bool IsSingleRowOrCol = CheckSingleRowOrColRemaining();

		if ( IsSingleRowOrCol && RouletteOutcome == ERouletteResult::Eliminate &&
			(SelectedRule == ERouletteRule::SameRowOnly || SelectedRule == ERouletteRule::SameColumnOnly) )
		{
			if ( bIsRouletteTestMode ) UE_LOG(LogLuckyDraw , Log , TEXT("한 행 또는 열만 남아 탈락을 방지합니다. 룰렛을 다시 돌립니다."));
			continue; // 한 행 또는 열만 남아있고, 탈락할 경우 전멸 방지를 위해 룰렛을 다시 돌림
		}
		else if ( IsSingleRowOrCol && RouletteOutcome == ERouletteResult::Pass &&
			(SelectedRule == ERouletteRule::ExcludeSameRow || SelectedRule == ERouletteRule::ExcludeSameColumn) )
		{
			if ( bIsRouletteTestMode ) UE_LOG(LogLuckyDraw , Log , TEXT("한 행 또는 열만 남아 통과로 전멸을 방지합니다. 룰렛을 다시 돌립니다."));
			continue;  // 한 행/열만 남고 통과일 경우 전멸 방지
		}

		// 전멸 방지 시뮬레이션
		TArray<int32> SimulatedRemainingPlayers = RemainingPlayers;

		// 탈락 처리 시뮬레이션 (실제 ApplyRouletteOutcome과 동일)
		if ( RouletteOutcome == ERouletteResult::Pass )
		{
			for ( int32 i = SimulatedRemainingPlayers.Num() - 1; i >= 0; --i )
			{
				if ( !AffectedPlayers.Contains(SimulatedRemainingPlayers[i]) )
				{
					//UE_LOG(LogLuckyDraw , Log , TEXT("통과로 인한 탈락자: %d") , SimulatedRemainingPlayers[i]);
					SimulatedRemainingPlayers.RemoveAt(i);
				}
			}
		}
		else if ( RouletteOutcome == ERouletteResult::Eliminate )
		{
			for ( int32 i = SimulatedRemainingPlayers.Num() - 1; i >= 0; --i )
			{
				if ( AffectedPlayers.Contains(SimulatedRemainingPlayers[i]) )
				{
					//UE_LOG(LogLuckyDraw , Log , TEXT("탈락으로 인한 탈락자: %d") , SimulatedRemainingPlayers[i]);
					SimulatedRemainingPlayers.RemoveAt(i);
				}
			}
		}

		// 시뮬레이션 좌석 업데이트
		UpdateSimulatedSeats(SimulatedSeats , SimulatedRemainingPlayers);  // 시뮬레이션 좌석 상태 갱신

		// 좌석 배열을 통해 모든 좌석이 'X'인지 확인
		bool bAllSeatsEmpty = true;
		for ( int32 i = 0; i < SimulatedSeats.Num(); ++i )
		{
			for ( int32 j = 0; j < SimulatedSeats[i].Num(); ++j )
			{
				if ( SimulatedSeats[i][j].PlayerNumber != -1 )  // 플레이어 번호를 기준으로 좌석이 비어 있는지 확인
				{
					bAllSeatsEmpty = false;
					break;
				}
			}
			if ( !bAllSeatsEmpty ) break;
		}

		// 모든 좌석이 비어 있으면 전멸 처리
		if ( bAllSeatsEmpty )
		{
			if ( bIsRouletteTestMode ) UE_LOG(LogLuckyDraw , Log , TEXT("전멸을 방지하기 위해 룰렛을 다시 돌립니다."));
			continue;
		}

		// 시뮬레이션 좌석 상태 업데이트 및 출력
		UpdateSimulatedSeats(SimulatedSeats , SimulatedRemainingPlayers);

		if ( SimulatedRemainingPlayers.Num() < 1 )
		{
			if ( bIsRouletteTestMode ) UE_LOG(LogLuckyDraw , Log , TEXT("전멸 방지를 위해 탈락을 방지합니다. 룰렛을 다시 돌립니다."));
			continue;
		}

		// 탈락자가 0명이면 다시 룰렛 돌리기
		if ( SimulatedRemainingPlayers.Num() == RemainingPlayers.Num() )
		{
			if ( bIsRouletteTestMode ) UE_LOG(LogLuckyDraw , Log , TEXT("탈락자가 없으므로 룰렛을 다시 돌립니다."));
			continue;
		}

		IsValidResult = true;
		//for ( int32 i = SimulatedRemainingPlayers.Num() - 1; i >= 0; --i )
		//{
		//	UE_LOG(LogLuckyDraw , Log , TEXT("남은 플레이어 체크: %d") , SimulatedRemainingPlayers[i]);
		//}

		// 룰렛 정보 저장
		FRouletteInfo RouletteInfo;
		RouletteInfo.Player = SelectedPlayer;
		RouletteInfo.Rule = SelectedRule;
		RouletteInfo.Result = RouletteOutcome;
		RouletteInfosPerRound.Add(RouletteInfo);
	}

	ApplyRouletteOutcome(SelectedPlayer , SelectedRule , RouletteOutcome);
}

// 시뮬레이션 중 좌석 출력 함수 추가
void ATTLuckyDrawGameMode::PrintSimulatedSeats(const TArray<TArray<FSeat>>& SimulatedSeats)
{
	if ( bIsRouletteTestMode ) UE_LOG(LogLuckyDraw , Log , TEXT("시뮬레이션 좌석 배치:"));
	for ( int32 i = 0; i < SimulatedSeats.Num(); ++i )
	{
		FString SeatRow;
		for ( int32 j = 0; j < SimulatedSeats[i].Num(); ++j )
		{
			if ( SimulatedSeats[i][j].PlayerNumber != -1 )
			{
				SeatRow += FString::Printf(TEXT("%d\t") , SimulatedSeats[i][j].PlayerNumber);
			}
			else
			{
				SeatRow += TEXT("X\t");
			}
		}
		if ( bIsRouletteTestMode ) UE_LOG(LogLuckyDraw , Log , TEXT("%s") , *SeatRow);
	}
}

// 시뮬레이션용 좌석 갱신 함수
void ATTLuckyDrawGameMode::UpdateSimulatedSeats(TArray<TArray<FSeat>>& SimulatedSeats , const TArray<int32>& SimulatedRemainingPlayers)
{
	for ( int32 i = 0; i < SimulatedSeats.Num(); ++i )  // i < 3
	{
		for ( int32 j = 0; j < SimulatedSeats[i].Num(); ++j )  // j < 10
		{
			if ( SimulatedSeats[i][j].PlayerNumber != -1 && !SimulatedRemainingPlayers.Contains(SimulatedSeats[i][j].PlayerNumber) )
			{
				SimulatedSeats[i][j].PlayerNumber = -1;  // 시뮬레이션에서 탈락한 플레이어 좌석 제거
			}
		}
	}

	// 시뮬레이션 좌석 상태 출력
	PrintSimulatedSeats(SimulatedSeats);
}

// 룰렛 결과를 적용하는 함수
void ATTLuckyDrawGameMode::ApplyRouletteOutcome(int32 Player , ERouletteRule Rule , ERouletteResult Outcome)
{
	// 첫 번째 룰렛 출력
	UE_LOG(LogLuckyDraw , Log , TEXT("룰렛1: %d") , Player);

	// 두 번째 룰렛 출력 (타이머 제거)
	UE_LOG(LogLuckyDraw , Log , TEXT("룰렛2: %d") , static_cast<int32>(Rule));

	// 세 번째 룰렛 출력 및 결과 적용 (타이머 제거)
	UE_LOG(LogLuckyDraw , Log , TEXT("룰렛3: %d") , static_cast<int32>(Outcome));

	// 영향을 받는 플레이어 리스트 생성
	TArray<int32> AffectedPlayers;
	GetAffectedPlayers(Player , Rule , AffectedPlayers);

	if ( Outcome == ERouletteResult::Pass )
	{
		// 통과 로직
		for ( int32 i = RemainingPlayers.Num() - 1; i >= 0; --i )
		{
			if ( !AffectedPlayers.Contains(RemainingPlayers[i]) )
			{
				if ( bIsRouletteTestMode ) UE_LOG(LogLuckyDraw , Log , TEXT("플레이어 %d 탈락") , RemainingPlayers[i]);
				RoundEliminatedPlayers.Add(RemainingPlayers[i]);
				RemainingPlayers.RemoveAt(i);
			}
		}
	}
	else if ( Outcome == ERouletteResult::Eliminate )
	{
		// 탈락 로직
		for ( int32 i = RemainingPlayers.Num() - 1; i >= 0; --i )
		{
			if ( AffectedPlayers.Contains(RemainingPlayers[i]) )
			{
				if ( bIsRouletteTestMode ) UE_LOG(LogLuckyDraw , Log , TEXT("플레이어 %d 탈락") , RemainingPlayers[i]);
				RoundEliminatedPlayers.Add(RemainingPlayers[i]);
				RemainingPlayers.RemoveAt(i);
			}
		}
	}

	// 좌석 갱신
	UpdateSeats();

	// 라운드가 끝나기 전에 탈락자 로그 출력
	if ( RoundEliminatedPlayers.Num() > 0 )
	{
		FString EliminatedPlayersString;
		for ( int32 SelectedPlayer : RoundEliminatedPlayers )
		{
			EliminatedPlayersString += FString::Printf(TEXT("%d ") , SelectedPlayer);
		}
		if ( bIsRouletteTestMode ) UE_LOG(LogLuckyDraw , Log , TEXT("라운드 %d 탈락자: %s") , Round , *EliminatedPlayersString);

		// 현재 라운드의 탈락자를 기록
		EliminatedPlayersPerRound.Add(RoundEliminatedPlayers);

		// 다음 라운드를 위해 초기화
		RoundEliminatedPlayers.Empty();
	}

	StartRound();
}

// 좌석 갱신 함수
void ATTLuckyDrawGameMode::UpdateSeats()
{
	for ( int32 i = 0; i < Seats.Num(); ++i )  // i < 3
	{
		for ( int32 j = 0; j < Seats[i].Num(); ++j )  // j < 10
		{
			if ( Seats[i][j].PlayerNumber != -1 && !RemainingPlayers.Contains(Seats[i][j].PlayerNumber) )
			{
				Seats[i][j].PlayerNumber = -1;  // 탈락자는 좌석에서 제거
			}
		}
	}
	PrintSeats();
}

void ATTLuckyDrawGameMode::GetAffectedPlayers(int32 SelectedPlayer , ERouletteRule Rule , TArray<int32>& AffectedPlayers)
{
	AffectedPlayers.Empty();  // 영향을 받는 플레이어 리스트 초기화

	// 선택된 플레이어의 좌석 위치 찾기
	int32 PlayerRow = -1;
	int32 PlayerCol = -1;
	for ( int32 i = 0; i < Seats.Num(); ++i )  // 행(가로)
	{
		for ( int32 j = 0; j < Seats[i].Num(); ++j )  // 열(세로)
		{
			if ( Seats[i][j].PlayerNumber == SelectedPlayer )
			{
				PlayerRow = i;
				PlayerCol = j;
				break;
			}
		}
		if ( PlayerRow != -1 ) break; // 플레이어를 찾으면 루프 종료
	}

	// 룰렛2의 규칙에 따라 영향을 받는 플레이어들을 찾아냄
	switch ( Rule )
	{
	case ERouletteRule::SameColumnOnly: // 같은 열에 있는 플레이어만 영향받음
		for ( int32 i = 0; i < Seats.Num(); ++i )  // 행 전체 탐색
		{
			if ( Seats[i][PlayerCol].IsOccupied() )
			{
				AffectedPlayers.Add(Seats[i][PlayerCol].PlayerNumber);  // 같은 열에 있는 플레이어 추가
			}
		}
		break;

	case ERouletteRule::SameRowOnly: // 같은 행에 있는 플레이어만 영향받음
		for ( int32 j = 0; j < Seats[PlayerRow].Num(); ++j )  // 열 전체 탐색
		{
			if ( Seats[PlayerRow][j].IsOccupied() )
			{
				AffectedPlayers.Add(Seats[PlayerRow][j].PlayerNumber);  // 같은 행에 있는 플레이어 추가
			}
		}
		break;

	case ERouletteRule::ExcludeSameColumn:  // 같은 열 제외
		// 같은 열 제외: 선택된 플레이어와 같은 열에 있는 플레이어는 탈락하지 않고, 다른 열에 있는 플레이어가 영향을 받음
		for ( int32 j = 0; j < Seats[0].Num(); ++j )  // 열 전체 탐색
		{
			if ( j != PlayerCol )  // 같은 열을 제외하고 다른 열 탐색
			{
				for ( int32 i = 0; i < Seats.Num(); ++i )  // 행 탐색
				{
					if ( Seats[i][j].IsOccupied() )
					{
						AffectedPlayers.Add(Seats[i][j].PlayerNumber);  // 다른 열에 있는 플레이어 추가
					}
				}
			}
		}
		break;

	case ERouletteRule::ExcludeSameRow:  // 같은 행 제외
		// 같은 행 제외: 선택된 플레이어와 같은 행에 있는 플레이어는 탈락하지 않고, 다른 행에 있는 플레이어가 영향을 받음
		for ( int32 i = 0; i < Seats.Num(); ++i )  // 행 전체 탐색
		{
			if ( i != PlayerRow )  // 같은 행을 제외하고 다른 행 탐색
			{
				for ( int32 j = 0; j < Seats[i].Num(); ++j )
				{
					if ( Seats[i][j].IsOccupied() )
					{
						AffectedPlayers.Add(Seats[i][j].PlayerNumber);  // 다른 행에 있는 플레이어 추가
					}
				}
			}
		}
		break;

	case ERouletteRule::OnlySelected:  // 선택된 플레이어만
		AffectedPlayers.Add(SelectedPlayer);  // 선택된 플레이어만 영향받음
		break;

	default:
		break;
	}
}

// 좌석을 무작위로 섞는 함수
void ATTLuckyDrawGameMode::ShuffleSeats()
{
	std::random_device rd;
	std::mt19937 g(rd());

	TArray<FVector2D> AvailableSeats;  // 사용할 수 있는 좌석 리스트

	// 좌석의 모든 가능한 위치(행, 열)를 초기화
	for ( int32 i = 0; i < 10; ++i )  // 행
	{
		for ( int32 j = 0; j < 3; ++j )  // 열
		{
			AvailableSeats.Add(FVector2D(i , j));
		}
	}

	// 사용할 수 있는 좌석을 무작위로 섞음
	std::shuffle(AvailableSeats.GetData() , AvailableSeats.GetData() + AvailableSeats.Num() , g);

	int32 Index = 0;
	// 무작위로 섞인 좌석에 플레이어 배치
	for ( int32 i = 0; i < AvailableSeats.Num() && Index < RemainingPlayers.Num(); ++i )
	{
		int32 Row = AvailableSeats[i].X;
		int32 Col = AvailableSeats[i].Y;

		if ( Seats[Col][Row].PlayerNumber == -1 )
		{
			Seats[Col][Row].PlayerNumber = RemainingPlayers[Index];
			Index++;
		}
	}
	
	SavedSeats = Seats;

	PrintSeats();
}

// 좌석을 출력하는 함수
void ATTLuckyDrawGameMode::PrintSeats()
{
	if ( bIsRouletteTestMode ) UE_LOG(LogLuckyDraw , Log , TEXT("좌석 배치:"));
	for ( int32 i = 0; i < 3; ++i )
	{
		FString SeatRow;
		for ( int32 j = 0; j < 10; ++j )
		{
			if ( Seats[i][j].IsOccupied() )
			{
				SeatRow += FString::Printf(TEXT("%d\t") , Seats[i][j].PlayerNumber);
			}
			else
			{
				SeatRow += TEXT("X\t");
			}
		}
		if ( bIsRouletteTestMode ) UE_LOG(LogLuckyDraw , Log , TEXT("%s") , *SeatRow);
	}
}

bool ATTLuckyDrawGameMode::IsGameOver() const
{
	return RemainingPlayers.Num() == 1;
}

bool ATTLuckyDrawGameMode::CheckSingleRowOrColRemaining()
{
	TSet<int32> Rows , Cols;

	// 남은 플레이어들이 있는 행과 열을 확인
	for ( int32 Player : RemainingPlayers )
	{
		int32 PlayerRow = -1 , PlayerCol = -1;
		GetPlayerPosition(Player , PlayerRow , PlayerCol);  // 플레이어의 위치를 찾음
		Rows.Add(PlayerRow);
		Cols.Add(PlayerCol);
	}

	// 하나의 행 또는 열에만 플레이어가 남아 있으면 true 반환
	return Rows.Num() == 1 || Cols.Num() == 1;
}

void ATTLuckyDrawGameMode::GetPlayerPosition(int32 Player , int32& OutRow , int32& OutCol)
{
	OutRow = -1;
	OutCol = -1;

	// 좌석 배열에서 해당 플레이어의 좌석 위치를 찾음
	for ( int32 Row = 0; Row < Seats.Num(); ++Row )  // 행 탐색
	{
		for ( int32 Col = 0; Col < Seats[Row].Num(); ++Col )  // 열 탐색
		{
			if ( Seats[Row][Col].PlayerNumber == Player )
			{
				OutRow = Row;
				OutCol = Col;
				return;  // 플레이어를 찾으면 함수를 종료
			}
		}
	}
}