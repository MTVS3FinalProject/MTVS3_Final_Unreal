// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/LuckyDrawManager.h"

#include "EngineUtils.h"
#include "Components/ChildActorComponent.h"
#include "HJ/LuckyDrawChair.h"
#include "HJ/TTLuckyDrawGameMode.h"
#include "HJ/TTLuckyDrawGameState.h"
#include "HJ/TTPlayer.h"

// Sets default values
ALuckyDrawManager::ALuckyDrawManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALuckyDrawManager::BeginPlay()
{
	Super::BeginPlay();

	// LuckyDrawChair 컴포넌트를 배열에 저장
	TArray<UChildActorComponent*> LuckyDrawChairComponents;
	GetComponents(LuckyDrawChairComponents);

	// 의자 컴포넌트 위치에 따른 태그 번호 부여를 위한 정렬 (10행 3열)
	LuckyDrawChairComponents.Sort([](const UChildActorComponent& A, const UChildActorComponent& B) {
		FVector LocationA = A.GetComponentLocation();
		FVector LocationB = B.GetComponentLocation();

		// X 값이 작을수록 앞열로 배치, 같은 X 값에서는 Y 값이 클수록 아래쪽 배치
		if (FMath::Abs(LocationA.Y - LocationB.Y) > KINDA_SMALL_NUMBER)
		{
			return LocationA.Y < LocationB.Y; // Y 값이 작은 것이 윗 행
		}
		return LocationA.X < LocationB.X;     // 같은 Y 값이면 X 값으로 비교
	});

	// 정렬된 컴포넌트들에 ChairTag 부여 (10행 3열 기준)
	for (int32 Index = 0; Index < LuckyDrawChairComponents.Num(); ++Index)
	{
		UChildActorComponent* ChildComponent = LuckyDrawChairComponents[Index];
		if (ChildComponent && ChildComponent->GetChildActor())
		{
			// 태그를 "Chair_1", "Chair_2", ..., "Chair_30" 형식으로 추가
			FString Tag = FString::Printf(TEXT("Chair_%d"), Index + 1);
			ChildComponent->ComponentTags.Add(FName(*Tag));

			UE_LOG(LogTemp, Log, TEXT("Added Tag: %s to Chair Component at index %d"), *Tag, Index + 1);
		}
	}
}

// Called every frame
void ALuckyDrawManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALuckyDrawManager::ConvertSeatAssignments()
{
	ATTLuckyDrawGameMode* GameMode = Cast<ATTLuckyDrawGameMode>(GetWorld()->GetAuthGameMode());
	if (!GameMode) return;

	const TArray<TArray<FSeat>>& ShuffledSeats = GameMode->GetShuffledSeats();
	PlayerToChairMap.Empty();

	UE_LOG(LogLuckyDraw, Log, TEXT("ConvertSeatAssignments: Converting ShuffledSeats to PlayerToChairMap..."));

	// 행 우선 순회 방식으로 10행 3열 구조로 ChairTagNumber를 부여
	int32 ChairTagNumber = 1;  // 태그 번호를 위한 변수

	for (int32 Row = 0; Row < ShuffledSeats.Num(); ++Row)  // 행 순회
	{
		FString RowLog;  // 각 행의 로그를 저장할 문자열

		for (int32 Col = 0; Col < ShuffledSeats[Row].Num(); ++Col)  // 열 순회
		{
			int32 SeatPlayerNumber = ShuffledSeats[Row][Col].PlayerNumber;
			if (SeatPlayerNumber != -1)
			{
				// 의자 태그를 순차적으로 생성
				FString ChairTag = FString::Printf(TEXT("Chair_%d"), ChairTagNumber);
				PlayerToChairMap.Add(SeatPlayerNumber, ChairTag);

				// 배정된 좌석의 ChairTagNumber를 RowLog에 추가
				RowLog += FString::Printf(TEXT("%d\t"), ChairTagNumber);
			}
			else
			{
				// 빈 좌석은 "X"로 표시
				RowLog += TEXT("X\t");
			}
			ChairTagNumber++;  // 의자 태그 번호를 순차 증가
		}
		// 각 행의 상태를 Warning 로그로 출력
		UE_LOG(LogLuckyDraw, Warning, TEXT("%s"), *RowLog);
	}

	UE_LOG(LogLuckyDraw, Log, TEXT("ConvertSeatAssignments: Completed."));
	MovePlayersToChairs();
}

void ALuckyDrawManager::MovePlayersToChairs()
{
	ATTLuckyDrawGameState* GameState = GetWorld()->GetGameState<ATTLuckyDrawGameState>();
	if (!GameState) return;

	TArray<UChildActorComponent*> ChairComponents;
	GetComponents(ChairComponents);

	// 각 플레이어를 순회하며 지정된 의자 위치로 이동
	for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
	{
		ATTPlayer* Player = *It;
		if (!Player || Player->GetbIsHost()) continue;

		// 플레이어의 SeatNumber로 PlayerToChairMap에서 TargetTag를 찾음
		int32 SeatNumber = Player->GetRandomSeatNumber();
		FString* TargetTag = PlayerToChairMap.Find(SeatNumber);
		if (!TargetTag) continue;

		// ChairComponents에서 TargetTag와 일치하는 의자를 찾아 이동
		for (UChildActorComponent* ChairComponent : ChairComponents)
		{
			if (ChairComponent && ChairComponent->ComponentTags.Contains(FName(**TargetTag)))
			{
				ALuckyDrawChair* TargetChair = Cast<ALuckyDrawChair>(ChairComponent->GetChildActor());
				if (TargetChair)
				{
					FTransform TargetTransform = TargetChair->GetSittingTransform();
					// Player->SetActorTransform(TargetTransform);
					Player->MulticastMovePlayerToChair(TargetTransform);
					UE_LOG(LogLuckyDraw, Log, TEXT("Player %s moved to %s"), *Player->GetNickname(), **TargetTag);
				}
				break;
			}
		}
	}
}