// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTLuckyDrawGameMode.h"
#include "TimerManager.h"
#include "random"  // std::shuffle와 무작위 엔진 사용을 위한 헤더 추가

// LogLuckyDraw 카테고리 정의
DEFINE_LOG_CATEGORY(LogLuckyDraw);

void ATTLuckyDrawGameMode::BeginPlay()
{
	Super::BeginPlay();

	if ( bIsRouletteTestMode )
	{
		// 인원 수를 출력
		if ( GEngine )
		{
			GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Yellow , FString::Printf(TEXT("설정된 인원 수: %d") , NumPlayers));
		}

		// 3*10 좌석 배열 초기화 (세로 3줄, 가로 10줄)
		Seats.SetNum(3);  // 열(세로) 수 3
		for ( int i = 0; i < 3; ++i )
		{
			Seats[i].SetNum(10);  // 각 열에 10개의 좌석
		}

		// 플레이어 번호 부여 및 랜덤 배치
		RemainingPlayers.Reserve(NumPlayers);
		for ( int32 i = 0; i < NumPlayers; ++i )
		{
			RemainingPlayers.Add(i + 1);  // 플레이어 번호는 1부터 시작
		}

		ShuffleSeats();

		// 첫 라운드 시작
		StartRound();
	}
}

// 라운드를 시작하는 함수
void ATTLuckyDrawGameMode::StartRound()
{
	if ( IsGameOver() )
	{
		UE_LOG(LogLuckyDraw , Log , TEXT("___________"));
		UE_LOG(LogLuckyDraw , Log , TEXT("총 라운드 수: %d") , Round);
		UE_LOG(LogLuckyDraw , Log , TEXT("최종 승자: %d") , RemainingPlayers[0]);
		return;
	}

	// 라운드가 끝나고 다음 라운드로 넘어가기 전에 출력
	UE_LOG(LogLuckyDraw , Log , TEXT("___________"));  // 로그 출력

	Round++;
	UE_LOG(LogLuckyDraw , Log , TEXT("\n라운드 수: %d") , Round);
	PrintSeats();

	// 룰렛을 돌리는 함수 호출
	GetWorld()->GetTimerManager().SetTimer(RouletteTimer , this , &ATTLuckyDrawGameMode::SelectRouletteOptions , EliminationEffectDuration , false);
}

// 룰렛 옵션을 선택하는 함수
void ATTLuckyDrawGameMode::SelectRouletteOptions()
{
	if ( RemainingPlayers.Num() == 0 )
	{
		UE_LOG(LogLuckyDraw , Error , TEXT("남은 플레이어가 없습니다. 룰렛을 돌릴 수 없습니다."));
		return;
	}

	bool IsValidResult = false;
	int32 SelectedPlayer = -1;
	FString RowOrColRule;
	FString PassOrFail;

	while ( !IsValidResult )
	{
		// 남아 있는 플레이어 중에서 랜덤하게 플레이어 선택
		int32 PlayerIndex = FMath::RandRange(0 , RemainingPlayers.Num() - 1);
		SelectedPlayer = RemainingPlayers[PlayerIndex];

		// 룰렛2: 행 또는 열 규칙 선택
		TArray<FString> RowOrColOptions = { TEXT("과(와) 같은 열만"), TEXT("과(와) 같은 행만"), TEXT("과(와) 같은 열 제외"), TEXT("과(와) 같은 행 제외"), TEXT("만") };
		int32 RowOrColIndex = FMath::RandRange(0 , RowOrColOptions.Num() - 1);
		RowOrColRule = RowOrColOptions[RowOrColIndex];

		// 룰렛3: 통과 또는 탈락 선택
		TArray<FString> PassOrFailOptions = { TEXT("통과"), TEXT("탈락") };
		int32 PassOrFailIndex = FMath::RandRange(0 , PassOrFailOptions.Num() - 1);
		PassOrFail = PassOrFailOptions[PassOrFailIndex];

		// 룰렛2 규칙에 따른 영향을 받는 플레이어 계산
		TArray<int32> AffectedPlayers;
		GetAffectedPlayers(SelectedPlayer , RowOrColRule , AffectedPlayers);

		// 한 행 또는 한 열만 남았을 때 전멸 방지 처리
		bool IsSingleRowOrCol = CheckSingleRowOrColRemaining();

		if ( IsSingleRowOrCol && PassOrFail == TEXT("탈락") &&
			(RowOrColRule.Contains(TEXT("과(와) 같은 행만")) || RowOrColRule.Contains(TEXT("과(와) 같은 열만"))) )
		{
			UE_LOG(LogLuckyDraw , Log , TEXT("한 행 또는 열만 남아 탈락을 방지합니다. 룰렛을 다시 돌립니다."));
			continue; // 한 행 또는 열만 남아있고, 탈락할 경우 전멸 방지를 위해 룰렛을 다시 돌림
		}
		else if ( IsSingleRowOrCol && PassOrFail == TEXT("통과") &&
			(RowOrColRule.Contains(TEXT("과(와) 같은 행 제외")) || RowOrColRule.Contains(TEXT("과(와) 같은 열 제외"))) )
		{
			UE_LOG(LogLuckyDraw , Log , TEXT("한 행 또는 열만 남아 통과로 전멸을 방지합니다. 룰렛을 다시 돌립니다."));
			continue;  // 한 행/열만 남고 통과일 경우 전멸 방지
		}

		// 전멸 방지 시뮬레이션
		TArray<int32> SimulatedRemainingPlayers = RemainingPlayers;

		// 탈락 처리 시뮬레이션 (실제 ApplyRouletteOutcome과 동일)
		if ( PassOrFail == TEXT("통과") )
		{
			// 통과: AffectedPlayers만 남고 나머지 탈락
			for ( int32 i = SimulatedRemainingPlayers.Num() - 1; i >= 0; --i )
			{
				if ( !AffectedPlayers.Contains(SimulatedRemainingPlayers[i]) )
				{
					SimulatedRemainingPlayers.RemoveAt(i);
				}
			}
		}
		else if ( PassOrFail == TEXT("탈락") )
		{
			// 탈락: AffectedPlayers 탈락, 나머지 통과
			for ( int32 i = SimulatedRemainingPlayers.Num() - 1; i >= 0; --i )
			{
				if ( AffectedPlayers.Contains(SimulatedRemainingPlayers[i]) )
				{
					SimulatedRemainingPlayers.RemoveAt(i);
				}
			}
		}

		if ( SimulatedRemainingPlayers.Num() < 1 )
		{
			UE_LOG(LogLuckyDraw , Log , TEXT("전멸 방지를 위해 탈락을 방지합니다. 룰렛을 다시 돌립니다."));
			continue;
		}

		IsValidResult = true;
	}

	ApplyRouletteOutcome(SelectedPlayer , RowOrColRule , PassOrFail);
}

// 룰렛 결과를 적용하는 함수
void ATTLuckyDrawGameMode::ApplyRouletteOutcome(int32 Player , const FString& RowOrColRule , const FString& PassOrFail)
{
	// 첫 번째 룰렛 출력 (즉시 출력)
	UE_LOG(LogLuckyDraw , Log , TEXT("룰렛1: %d") , Player);

	// 1.5초 후 두 번째 룰렛 출력
	GetWorld()->GetTimerManager().SetTimer(RouletteTimer , [this , Player , RowOrColRule]()
	{
		// 두 번째 룰렛 출력
		UE_LOG(LogLuckyDraw , Log , TEXT("룰렛2: %s") , *RowOrColRule);

		// 1.5초 후 세 번째 룰렛 출력
		GetWorld()->GetTimerManager().SetTimer(RouletteTimer , [this , Player , RowOrColRule]()
		{
			// 세 번째 룰렛 출력 및 결과 적용
			FString PassOrFail = FMath::RandBool() ? TEXT("통과") : TEXT("탈락");
			UE_LOG(LogLuckyDraw , Log , TEXT("룰렛3: %s") , *PassOrFail);

			// 영향을 받는 플레이어 리스트 생성
			TArray<int32> AffectedPlayers;
			GetAffectedPlayers(Player , RowOrColRule , AffectedPlayers);

			if ( PassOrFail == TEXT("통과") )
			{
				// AffectedPlayers가 탈락, 나머지는 통과
				for ( int32 i = RemainingPlayers.Num() - 1; i >= 0; --i )
				{
					if ( !AffectedPlayers.Contains(RemainingPlayers[i]) )
					{
						UE_LOG(LogLuckyDraw , Log , TEXT("플레이어 %d 탈락") , RemainingPlayers[i]);
						RemainingPlayers.RemoveAt(i);
					}
				}
			}
			else if ( PassOrFail == TEXT("탈락") )
			{
				// AffectedPlayers만 탈락, 나머지는 통과
				for ( int32 i = RemainingPlayers.Num() - 1; i >= 0; --i )
				{
					if ( AffectedPlayers.Contains(RemainingPlayers[i]) )
					{
						UE_LOG(LogLuckyDraw , Log , TEXT("플레이어 %d 탈락") , RemainingPlayers[i]);
						RemainingPlayers.RemoveAt(i);
					}
				}
			}

			// 좌석 갱신
			UpdateSeats();

			// 게임 종료 조건 체크
			if ( RemainingPlayers.Num() == 1 )
			{
				UE_LOG(LogLuckyDraw , Log , TEXT("___________"));
				UE_LOG(LogLuckyDraw , Log , TEXT("최종 승자: %d") , RemainingPlayers[0]);
				UE_LOG(LogLuckyDraw , Log , TEXT("총 라운드 수: %d") , Round);
				return;
			}

			// 다음 라운드 시작
			GetWorld()->GetTimerManager().SetTimer(RouletteTimer , this , &ATTLuckyDrawGameMode::StartRound , 3.0f , false);

		} , RouletteDelay , false);

	} , RouletteDelay , false);
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

void ATTLuckyDrawGameMode::GetAffectedPlayers(int32 SelectedPlayer , const FString& RowOrColRule , TArray<int32>& AffectedPlayers)
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
		if ( PlayerRow != -1 ) break;  // 플레이어를 찾으면 루프 종료
	}

	// 룰렛2의 규칙에 따라 영향을 받는 플레이어들을 찾아냄
	if ( RowOrColRule.Contains(TEXT("과(와) 같은 열만")) )  // 같은 열에 있는 플레이어만 영향받음
	{
		for ( int32 i = 0; i < Seats.Num(); ++i )  // 행 전체 탐색
		{
			if ( Seats[i][PlayerCol].IsOccupied() )
			{
				AffectedPlayers.Add(Seats[i][PlayerCol].PlayerNumber);  // 같은 열에 있는 플레이어 추가
			}
		}
	}
	else if ( RowOrColRule.Contains(TEXT("과(와) 같은 행만")) )  // 같은 행에 있는 플레이어만 영향받음
	{
		for ( int32 j = 0; j < Seats[PlayerRow].Num(); ++j )  // 열 전체 탐색
		{
			if ( Seats[PlayerRow][j].IsOccupied() )
			{
				AffectedPlayers.Add(Seats[PlayerRow][j].PlayerNumber);  // 같은 행에 있는 플레이어 추가
			}
		}
	}
	else if ( RowOrColRule.Contains(TEXT("과(와) 같은 열 제외")) )  // 같은 열 제외
	{
		//for ( int32 i = 0; i < Seats.Num(); ++i )
		//{
		//	if ( Seats[i][PlayerCol].IsOccupied() && i != PlayerRow )  // 열만 비교하여 영향받음
		//	{
		//		AffectedPlayers.Add(Seats[i][PlayerCol].PlayerNumber);  // 같은 열을 제외한 플레이어만 영향받음
		//	}
		//}

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
	}
	else if ( RowOrColRule.Contains(TEXT("과(와) 같은 행 제외")) )  // 같은 행 제외
	{
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
	}
	else if ( RowOrColRule.Contains(TEXT("만")) )  // 선택된 플레이어만
	{
		AffectedPlayers.Add(SelectedPlayer);  // 선택된 플레이어만 영향받음
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

	PrintSeats();
}

// 좌석을 출력하는 함수
void ATTLuckyDrawGameMode::PrintSeats()
{
	UE_LOG(LogLuckyDraw , Log , TEXT("좌석 배치:"));
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
		UE_LOG(LogLuckyDraw , Log , TEXT("%s") , *SeatRow);
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