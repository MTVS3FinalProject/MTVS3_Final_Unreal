#include "HJ/TTLuckyDrawGameState.h"
#include "HJ/TTGameInstance.h"
#include <HJ/TTPlayer.h>
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "HJ/LDTableManager.h"
#include "HJ/LuckyDrawChair.h"
#include "HJ/LuckyDrawManager.h"
#include "HJ/LuckyDrawTable.h"
#include "HJ/TTLuckyDrawGameMode.h"
#include "JMH/MH_CountDownActor.h"
#include "JMH/MH_GameWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ATTLuckyDrawGameState::ATTLuckyDrawGameState()
{
	
}

void ATTLuckyDrawGameState::BeginPlay()
{
	Super::BeginPlay();

	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (GI) GI->SetPlaceState(EPlaceState::LuckyDrawRoom);

	CurrentSeatNumber = 0;

	GameUI = CastChecked<UMH_GameWidget>(CreateWidget(GetWorld() , GameUIFactory));
	if (GameUI)
	{
		GameUI->AddToViewport();
		GameUI->SetWidgetSwitcher(0);
	}

	for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
	{
		ATTPlayer* TTPlayer = *It;
		if (TTPlayer && !TTPlayer->GetbIsHost() && TTPlayer->IsLocallyControlled())
		{
			TTPlayer->InitGameUI();
		}
	}

	// 타이머 핸들 초기화
	GetWorldTimerManager().ClearTimer(RoundTimerHandle);
	GetWorldTimerManager().ClearTimer(LuckyDrawLoseTimerHandle);
}

void ATTLuckyDrawGameState::ResetChair()
{
}

void ATTLuckyDrawGameState::AssignSeatNumber(APlayerState* PlayerState)
{
	ATTLuckyDrawGameMode* GameMode = GetWorld()->GetAuthGameMode<ATTLuckyDrawGameMode>();
	if (!GameMode) return;

	for (APlayerState* PS : PlayerArray) // 서버 클라이언트를 포함하기 위해 
	{
		ATTPlayer* TTPlayer = Cast<ATTPlayer>(PS->GetPawn());
		if (TTPlayer && !TTPlayer->GetbIsHost() && TTPlayer->GetRandomSeatNumber() == 0)
		{
			TTPlayer->SetRandomSeatNumber(++CurrentSeatNumber);

			// 디버그 메시지
			GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Green ,
			                                 FString::Printf(TEXT("플레이어 %s에게 좌석 번호 %d가 할당되었습니다.") ,
			                                                 *TTPlayer->GetNickname() ,
			                                                 TTPlayer->GetRandomSeatNumber()));

			NewSeatNumber = TTPlayer->GetRandomSeatNumber();
			// TTPlayer->ClientSetRandomSeatNumber(NewSeatNumber);

			if (HasAuthority() && TTPlayer->GameUI) TTPlayer->GameUI->SetTextMyNum(TTPlayer->GetRandomSeatNumber());

			if (GameUI)
			{
				GameUI->SetTextCurrentPlayer(NewSeatNumber);
			}
		}
	}
}

void ATTLuckyDrawGameState::StartLuckyDraw()
{
	ATTLuckyDrawGameMode* GameMode = GetWorld()->GetAuthGameMode<ATTLuckyDrawGameMode>();
	if (!GameMode) return;
	// GameMode->StartLuckyDraw(CurrentSeatNumber);
	GameMode->StartLuckyDraw(30);
}

void ATTLuckyDrawGameState::MovePlayersToChairs()
{
	bIsStartRound = true;

    ALuckyDrawManager* ChairManager = Cast<ALuckyDrawManager>(
        UGameplayStatics::GetActorOfClass(GetWorld(), ALuckyDrawManager::StaticClass()));
    ALDTableManager* TableManager = Cast<ALDTableManager>(
        UGameplayStatics::GetActorOfClass(GetWorld(), ALDTableManager::StaticClass()));

    if (!ChairManager || !TableManager) return;

    if (ChairManager)
    {
        ChairManager->ConvertSeatAssignments();
        UE_LOG(LogTemp, Log, TEXT("Lucky Draw Started: Players moved to their designated chairs."));
    }

    // 먼저 모든 테이블을 파란색으로 설정 (플레이어 루프 밖으로 이동)
    TArray<UChildActorComponent*> TableComponents;
    TableManager->GetComponents(TableComponents);
    for (UChildActorComponent* TableComponent : TableComponents)
    {
        ALuckyDrawTable* Table = Cast<ALuckyDrawTable>(TableComponent->GetChildActor());
        if (Table)
        {
            Table->SetColorBlue();  // 모든 테이블을 파란색으로 설정
            Table->MulticastSetTextRender(FText::FromString(TEXT(" "))); // 텍스트 초기화
        }
    }

    // 플레이어 처리
    for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
    {
        ATTPlayer* TTPlayer = *It;
        if (TTPlayer && !TTPlayer->GetbIsHost())
        {
            TTPlayer->MulticastLuckyDrawStart();
        }

        int32 SeatNumber = TTPlayer->GetRandomSeatNumber();
        // 플레이어가 있는 테이블에만 번호 표시
        if (SeatNumber != 0)  // SeatNumber가 유효한지 확인
        {
            FString* ChairTag = ChairManager->PlayerToChairMap.Find(SeatNumber);
            if (ChairTag)
            {
                FString TableTag = "Table_" + ChairTag->RightChop(6);
                for (UChildActorComponent* TableComponent : TableComponents)
                {
                    if (TableComponent && TableComponent->ComponentTags.Contains(FName(*TableTag)))
                    {
                        ALuckyDrawTable* TargetTable = Cast<ALuckyDrawTable>(TableComponent->GetChildActor());
                        if (TargetTable)
                        {
                            TargetTable->MulticastSetTextRender(FText::FromString(FString::FromInt(SeatNumber)));
                            UE_LOG(LogTemp, Log, TEXT("Seat number %d displayed on table %s"), SeatNumber, *TableTag);
                        }
                        break;
                    }
                }
            }
        }
    }

    // 카운트 다운 시작
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMH_CountDownActor::StaticClass(), FoundActors);
    if (FoundActors.Num() > 0)
    {
        AMH_CountDownActor* CountDownActor = Cast<AMH_CountDownActor>(FoundActors[0]);
        if (CountDownActor)
        {
            CountDownActor->MulticastStartCountDownVisible(true);
        }
    }
    MulticastShowOnlyNumPlayers();

    FTimerHandle RouletteTimerHandle;
    GetWorldTimerManager().SetTimer(RouletteTimerHandle, this, 
        &ATTLuckyDrawGameState::StartPlayRoulette, StartPlayRouletteDelayTime, false);
}

void ATTLuckyDrawGameState::StartPlayRoulette()
{
	MulticastStartLuckyDraw();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld() , AMH_CountDownActor::StaticClass() , FoundActors);
	if (FoundActors.Num() > 0)
	{
		AMH_CountDownActor* CountDownActor = Cast<AMH_CountDownActor>(FoundActors[0]);
		if (CountDownActor)
		{
			CountDownActor->MulticastStartCountDownVisible(false);
		}
	}

	ATTLuckyDrawGameMode* GameMode = GetWorld()->GetAuthGameMode<ATTLuckyDrawGameMode>();
	if (!GameMode) return;

	// 마지막 라운드까지 포함하도록 Round + 1을 전달
	StartRounds(GameMode->Round + 1);
}

void ATTLuckyDrawGameState::StartRounds(int32 InTotalRounds)
{
	// 타이머 초기화
	GetWorldTimerManager().ClearTimer(RoundTimerHandle);
	GetWorldTimerManager().ClearTimer(LuckyDrawLoseTimerHandle);
    
	TotalRounds = InTotalRounds;
	CurrentRound = 0;
    
	UE_LOG(LogTemp, Log, TEXT("Starting Rounds - Total Rounds: %d"), TotalRounds);
    
	// GameMode의 라운드 정보 개수 체크는 제거하고 바로 시작
	// 룰렛 정보는 라운드가 진행되면서 생성되기 때문에
	// 시작 시점에는 RouletteInfosPerRound가 비어있는 것이 정상입니다.
	StartNextRound();
}

void ATTLuckyDrawGameState::StartNextRound()
{
	// FTimerHandle LuckyDrawLoseTimerHandle;
	// GetWorldTimerManager().SetTimer(LuckyDrawLoseTimerHandle , this , &ATTLuckyDrawGameState::EliminatePlayers , 4.5f ,
	//                                 false);
	//
	// if (CurrentRound >= TotalRounds)
	// {
	// 	FTimerHandle RouletteTimerHandle;
	// 	GetWorldTimerManager().SetTimer(RouletteTimerHandle , this , &ATTLuckyDrawGameState::EndRounds , 8.0f , false);
	// 	return;
	// }
	// Clear any existing EliminatePlayers timer
	GetWorldTimerManager().ClearTimer(LuckyDrawLoseTimerHandle);

	// 마지막 라운드에서 실행
	if (CurrentRound >= TotalRounds)
	{
		// Set final elimination timer and end game
		GetWorldTimerManager().SetTimer(LuckyDrawLoseTimerHandle, this, 
			&ATTLuckyDrawGameState::EliminatePlayers, EliminatePlayersDelayTime, false);
		FTimerHandle RouletteTimerHandle;
		GetWorldTimerManager().SetTimer(RouletteTimerHandle, this, 
			&ATTLuckyDrawGameState::EndRounds, EndRoundsDelayTime, false);
		return;
	}

	// 일반 라운드에서 실행
	// Set timer for next elimination
	GetWorldTimerManager().SetTimer(LuckyDrawLoseTimerHandle, this, 
		&ATTLuckyDrawGameState::EliminatePlayers, EliminatePlayersDelayTime, false);

	// Set timer for next roulette round
	float Delay = (CurrentRound == 0) ? FirstPlayRouletteDelayTime : PlayRouletteDelayTime;
	GetWorldTimerManager().SetTimer(RoundTimerHandle, this, 
		&ATTLuckyDrawGameState::PlayRoulette, Delay, false);

	/*ATTLuckyDrawGameMode* GameMode = GetWorld()->GetAuthGameMode<ATTLuckyDrawGameMode>();
	if (!GameMode) return;

	// 각 라운드에 맞는 룰렛 정보를 가져옴
	const FRouletteInfo& Info = GameMode->GetRouletteInfoForRound(CurrentRound);

	// GameUI에 룰렛 정보를 반영
	if (GameUI)
	{ 
	    GameUI->SetTextroulette1(FString::FromInt(Info.Player));
	    GameUI->SetTextroulette2(static_cast<int32>(Info.Rule));
	    GameUI->SetTextroulette3(static_cast<int32>(Info.Result));
	}*/


	// 첫 라운드는 즉시 시작, 이후 라운드는 7초 딜레이
	// float Delay = (CurrentRound == 0) ? 0.5f : 10.0f;
	// GetWorldTimerManager().SetTimer(RoundTimerHandle , this , &ATTLuckyDrawGameState::PlayRoulette , Delay , false);
}

void ATTLuckyDrawGameState::MulticastPlayRouletteAnimation_Implementation()
{
	if (GameUI)
	{
		GameUI->PlayRouletteAnim();
	}
}

void ATTLuckyDrawGameState::OnRep_NewSeatNumber()
{
	if (GameUI)
	{
		GameUI->SetTextCurrentPlayer(NewSeatNumber);
	}
}

void ATTLuckyDrawGameState::MulticastPlayRouletteEndSound_Implementation()
{
	UGameplayStatics::PlaySound2D(GetWorld(), RouletteEndSound);
}

void ATTLuckyDrawGameState::MulticastShowOnlyNumPlayers_Implementation()
{
	if (GameUI)
	{
		GameUI->SetWidgetSwitcher(1);
		GameUI->SetCurrentPlayerVisible();
	}
}

void ATTLuckyDrawGameState::MulticastUpdatePlayerNumUI_Implementation(int32 PlayerNum)
{
	PlayerNum = NewSeatNumber;
}

void ATTLuckyDrawGameState::EliminatePlayers()
{
	MulticastPlayRouletteEndSound();
	
	ATTLuckyDrawGameMode* GameMode = GetWorld()->GetAuthGameMode<ATTLuckyDrawGameMode>();
	if (!GameMode) return;
	if (CurrentRound > 0 && GameMode->EliminatedPlayersPerRound.Num() >= CurrentRound)
	{
		int32 LastRoundIndex = CurrentRound - 1;
		ALuckyDrawManager* ChairManager = Cast<ALuckyDrawManager>(
			UGameplayStatics::GetActorOfClass(GetWorld() , ALuckyDrawManager::StaticClass()));
		ALDTableManager* TableManager = Cast<ALDTableManager>(
			UGameplayStatics::GetActorOfClass(GetWorld() , ALDTableManager::StaticClass()));

		if (!ChairManager || !TableManager) return;

		 // 탈락자 번호 배열
        const TArray<int32>& EliminatedNumbers = GameMode->EliminatedPlayersPerRound[LastRoundIndex];
        UE_LOG(LogTemp, Log, TEXT("Processing eliminated numbers in round %d"), LastRoundIndex + 1);
        
        // 현재 실제 좌석 배치 상태 가져오기
        const TArray<TArray<FSeat>>& CurrentSeats = GameMode->GetShuffledSeats();

        // 탈락된 번호들의 의자 태그를 찾기 위한 배열
        TArray<FString> ChairTagsToThrow;
        
        // 1. 탈락된 번호들의 실제 의자 태그를 찾기
        for (int32 EliminatedNumber : EliminatedNumbers)
        {
            UE_LOG(LogTemp, Log, TEXT("Looking for eliminated player: %d"), EliminatedNumber);
            bool bFound = false;
            // CurrentSeats에서 탈락한 번호의 위치 찾기
            for (int32 i = 0; i < CurrentSeats.Num() && !bFound; ++i)
            {
                for (int32 j = 0; j < CurrentSeats[i].Num() && !bFound; ++j)
                {
                    if (CurrentSeats[i][j].PlayerNumber == EliminatedNumber)
                    {
                        // 배치가 3x10 형태이므로 다음과 같이 계산
                        int32 ChairNumber = i * 10 + j + 1;
                        FString ChairTag = FString::Printf(TEXT("Chair_%d"), ChairNumber);
                        ChairTagsToThrow.Add(ChairTag);
                        UE_LOG(LogTemp, Log, TEXT("Found player %d at position (%d,%d), Chair_%d"), 
                            EliminatedNumber, i, j, ChairNumber);
                        bFound = true;
                    }
                }
            }
        }

		// 2. 테이블 색상 변경 (플레이어 처리 전에 수행)
		for (const FString& ChairTag : ChairTagsToThrow)
		{
			// Chair_ 태그를 Table_ 태그로 변환
			FString TableTag = TEXT("Table_") + ChairTag.RightChop(6);
			TArray<UChildActorComponent*> TableComponents;
			TableManager->GetComponents(TableComponents);

			for (UChildActorComponent* TableComponent : TableComponents)
			{
				if (TableComponent && TableComponent->ComponentTags.Contains(FName(*TableTag)))
				{
					if (ALuckyDrawTable* TargetTable = Cast<ALuckyDrawTable>(TableComponent->GetChildActor()))
					{
						TargetTable->SetColorRed();
						UE_LOG(LogTemp, Log, TEXT("Table %s color set to red"), *TableTag);
					}
					break;
				}
			}
		}

        // 3. 플레이어 UI/애니메이션 처리
        for (int32 PlayerID : EliminatedNumbers)
        {
            for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
            {
                ATTPlayer* Player = *It;
                if (Player && Player->GetRandomSeatNumber() == PlayerID)
                {
                    Player->ClientLuckyDrawLose();
                    // Player->MulticastSetColorTextRender(FColor(255, 0, 235));

                    // 탈락한 플레이어의 의자 태그 찾기
                    FString* TargetChairTag = ChairManager->PlayerToChairMap.Find(PlayerID);
                    if (TargetChairTag)
                    {
                        // 플레이어를 의자에 부착
                        TArray<UChildActorComponent*> ChairComponents;
                        ChairManager->GetComponents(ChairComponents);

                        for (UChildActorComponent* ChairComponent : ChairComponents)
                        {
                            if (ChairComponent && ChairComponent->ComponentTags.Contains(FName(**TargetChairTag)))
                            {
                                if (ALuckyDrawChair* TargetChair = Cast<ALuckyDrawChair>(ChairComponent->GetChildActor()))
                                {
                                	// 플레이어의 모든 콜리전을 비활성화
                                	Player->SetActorEnableCollision(false);
    
                                	if (UCharacterMovementComponent* MovementComp = Player->GetCharacterMovement())
                                	{
                                		// 캐릭터 무브먼트의 중력 스케일을 증가시켜 빠르게 떨어지도록 함
                                		MovementComp->GravityScale = 2.0f;
                                		// 캐릭터 무브먼트의 이동 모드를 Falling으로 변경
                                		MovementComp->SetMovementMode(MOVE_Falling);
                                	}
                                	
                                    // Player->AttachToActor(TargetChair, FAttachmentTransformRules::KeepWorldTransform);
                                    UE_LOG(LogTemp, Log, TEXT("Player %d attached and thrown with chair %s"), 
                                        PlayerID, **TargetChairTag);
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }

        // 4. 찾은 의자들 날리기
        UE_LOG(LogTemp, Log, TEXT("Found %d chairs to throw"), ChairTagsToThrow.Num());
        TArray<UChildActorComponent*> ChairComponents;
        ChairManager->GetComponents(ChairComponents);

		for (const FString& ChairTag : ChairTagsToThrow)
		{
			bool bFoundChair = false;
			for (UChildActorComponent* ChairComponent : ChairComponents)
			{
				if (!ChairComponent) continue;

				if (ChairComponent->ComponentTags.Contains(FName(*ChairTag)))
				{
					if (ALuckyDrawChair* Chair = Cast<ALuckyDrawChair>(ChairComponent->GetChildActor()))
					{
						// 0~3초 사이의 랜덤한 딜레이 생성
						float RandomDelay = FMath::FRandRange(0.0f, 1.5f);
                
						// 타이머를 통해 랜덤한 시간 후에 피직스 활성화
						FTimerHandle ThrowTimerHandle;
						FTimerDelegate TimerCallback;
                
						// 람다를 사용하여 현재 Chair 포인터를 캡처
						TimerCallback.BindLambda([Chair]()
						{
							if (Chair)
							{
								Chair->MulticastSetPhysicsState(true);
							}
						});
                
						GetWorld()->GetTimerManager().SetTimer(
							ThrowTimerHandle, 
							TimerCallback, 
							RandomDelay, 
							false);
                
						UE_LOG(LogTemp, Log, TEXT("Chair %s will be thrown after %.2f seconds"), 
							*ChairTag, RandomDelay);
						bFoundChair = true;
					}
					break;
				}
			}
            
            if (!bFoundChair)
            {
                UE_LOG(LogTemp, Warning, TEXT("Could not find chair with tag: %s"), *ChairTag);
            }
        }
    }
}

void ATTLuckyDrawGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATTLuckyDrawGameState , NewSeatNumber);
}

void ATTLuckyDrawGameState::MulticastStartLuckyDraw_Implementation()
{
	GameUI->ShowWidget();
	GameUI->PlayRouletteAnim04();
}

void ATTLuckyDrawGameState::PlayRoulette()
{
	ATTLuckyDrawGameMode* GameMode = GetWorld()->GetAuthGameMode<ATTLuckyDrawGameMode>();
	if (!GameMode) 
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode is invalid"));
		return;
	}

	// CurrentRound가 유효한지 확인
	if (CurrentRound >= TotalRounds)
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempting to play invalid round %d (Total rounds: %d)"), 
			CurrentRound + 1, TotalRounds);
		EndRounds();  // 강제로 게임 종료
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Playing Roulette for Round: %d"), CurrentRound + 1);

	// 룰렛 정보 가져오기
	const FRouletteInfo& Info = GameMode->GetRouletteInfoForRound(CurrentRound);
    
	// 유효하지 않은 플레이어 번호 체크
	if (Info.Player == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid roulette info for round %d, ending game"), CurrentRound + 1);
		EndRounds();  // 강제로 게임 종료
		return;
	}

	// UI 업데이트
	MulticastUpdateRouletteUI(Info.Player, static_cast<int32>(Info.Rule), static_cast<int32>(Info.Result));

	if (GameUI)
	{
		MulticastPlayRouletteAnimation();
	}

	CurrentRound++;
    
	// 다음 라운드 시작 전에 유효성 체크
	if (CurrentRound >= TotalRounds)
	{
		EndRounds();
	}
	else
	{
		StartNextRound();
	}
}

void ATTLuckyDrawGameState::MulticastHideGameUI_Implementation()
{
	GameUI->HideWidget();
}

void ATTLuckyDrawGameState::MulticastUpdateRouletteUI_Implementation(int32 Player , int32 Rule , int32 Result)
{
	if (GameUI)
	{
		GameUI->SetTextroulette1(FString::FromInt(Player));
		GameUI->SetTextroulette2(Rule);
		GameUI->SetTextroulette3(Result);
	}
}

void ATTLuckyDrawGameState::EndRounds()
{
	ATTLuckyDrawGameMode* GameMode = GetWorld()->GetAuthGameMode<ATTLuckyDrawGameMode>();
	if (!GameMode || GameMode->RemainingPlayers.Num() == 0) return;

	MulticastEndRounds();

	// 우승자 플레이어의 좌석 번호
	int32 WinningSeatNumber = GameMode->RemainingPlayers[0];

	// 우승자 찾기
	for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
	{
		ATTPlayer* TTPlayer = *It;
		// TTPlayer->MulticastSetVisibilityTextRender(false);
		if (TTPlayer && TTPlayer->GetRandomSeatNumber() == WinningSeatNumber)
		{
			TTPlayer->ClientLuckyDrawWin(); // 우승자에게만 종료 호출
			// TTPlayer->MulticastSetColorTextRender(FColor(0 , 220 , 255));
			break;
		}
	}
	
	// 모든 테이블을 검정색으로 설정하고 텍스트 초기화
	ALDTableManager* TableManager = Cast<ALDTableManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ALDTableManager::StaticClass()));
	
	if (TableManager)
	{
		TArray<UChildActorComponent*> TableComponents;
		TableManager->GetComponents(TableComponents);

		for (UChildActorComponent* TableComponent : TableComponents)
		{
			ALuckyDrawTable* Table = Cast<ALuckyDrawTable>(TableComponent->GetChildActor());
			if (Table)
			{
				Table->SetColorBlack(); // 테이블 색상을 검정색으로 설정
				Table->MulticastSetTextRender(FText::FromString(TEXT(" "))); // 텍스트 초기화
			}
		}
	}

	// 좌석 번호 초기화
	CurrentSeatNumber = 0;

	// 게임 종료 시 타이머 정리
	GetWorldTimerManager().ClearTimer(RoundTimerHandle);
	GetWorldTimerManager().ClearTimer(LuckyDrawLoseTimerHandle);

	FTimerHandle EndLuckyDrawTimerHandle;
	GetWorldTimerManager().SetTimer(EndLuckyDrawTimerHandle , this , &ATTLuckyDrawGameState::InitializeChairs , 5.0f ,
									false);
}

void ATTLuckyDrawGameState::InitializeChairs()
{
	// 모든 의자 원위치 및 테이블 초기화
	ALuckyDrawManager* ChairManager = Cast<ALuckyDrawManager>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ALuckyDrawManager::StaticClass()));
	
	if (ChairManager)
	{
		TArray<UChildActorComponent*> ChairComponents;
		ChairManager->GetComponents(ChairComponents);
	
		for (UChildActorComponent* ChairComponent : ChairComponents)
		{
			if (ALuckyDrawChair* Chair = Cast<ALuckyDrawChair>(ChairComponent->GetChildActor()))
			{
				Chair->MulticastResetChair();
				// Chair->ResetChair(); // LuckyDrawChair에 추가할 함수
			}
		}
	}

	bIsStartRound = false;
}

void ATTLuckyDrawGameState::MulticastEndRounds_Implementation()
{
	if (GameUI)
	{
		GameUI->HideWidget();
	}
}
