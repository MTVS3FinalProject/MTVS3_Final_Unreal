#include "HJ/TTLuckyDrawGameState.h"
#include "HJ/TTGameInstance.h"
#include <HJ/TTPlayer.h>
#include "EngineUtils.h"
#include "GameFramework/PlayerState.h"
#include "HJ/LDTableManager.h"
#include "HJ/LuckyDrawChair.h"
#include "HJ/LuckyDrawLine.h"
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
	GameMode->StartLuckyDraw(CurrentSeatNumber);
}

void ATTLuckyDrawGameState::MovePlayersToChairs()
{
	bIsStartRound = true;

	ALuckyDrawManager* ChairManager = Cast<ALuckyDrawManager>(
		UGameplayStatics::GetActorOfClass(GetWorld() , ALuckyDrawManager::StaticClass()));
	ALDTableManager* TableManager = Cast<ALDTableManager>(
		UGameplayStatics::GetActorOfClass(GetWorld() , ALDTableManager::StaticClass()));

	if (!ChairManager || !TableManager) return;

	if (ChairManager)
	{
		ChairManager->ConvertSeatAssignments();
		UE_LOG(LogTemp , Log , TEXT("Lucky Draw Started: Players moved to their designated chairs."));
	}

	for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
	{
		ATTPlayer* TTPlayer = *It;
		if (TTPlayer && !TTPlayer->GetbIsHost())
		{
			TTPlayer->MulticastLuckyDrawStart();
			// TextRenderComp 보이게 하기
			// TTPlayer->MulticastSetVisibilityTextRender(true);
		}


		int32 SeatNumber = TTPlayer->GetRandomSeatNumber();
		if (SeatNumber != 0)  // SeatNumber가 유효한지 확인
		{
			FString* ChairTag = ChairManager->PlayerToChairMap.Find(SeatNumber);
			if (ChairTag)
			{
				// Chair_ 형태에서 Table_ 형태로 변환
				FString TableTag = "Table_" + ChairTag->RightChop(6);
				TArray<UChildActorComponent*> TableComponents;
				TableManager->GetComponents(TableComponents);

				for (UChildActorComponent* TableComponent : TableComponents)
				{
					if (TableComponent && TableComponent->ComponentTags.Contains(FName(*TableTag)))
					{
						ALuckyDrawTable* TargetTable = Cast<ALuckyDrawTable>(TableComponent->GetChildActor());
						if (TargetTable)
						{
							TargetTable->SetColorBlue();  // 테이블 색상을 파란색으로 설정
							TargetTable->MulticastSetTextRender(FText::FromString(FString::FromInt(SeatNumber)));
							UE_LOG(LogTemp, Log, TEXT("Table %s color set to blue and seat number %d displayed"), *TableTag, SeatNumber);
						}
						break;
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("ChairTag not found for SeatNumber: %d"), SeatNumber);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Invalid SeatNumber (0) for player: %s"), *TTPlayer->GetNickname());
		}
	}


	// 카운트 다운 시작
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld() , AMH_CountDownActor::StaticClass() , FoundActors);
	if (FoundActors.Num() > 0)
	{
		// 첫 번째 찾은 액터를 AMH_CountDownActor로 캐스팅
		AMH_CountDownActor* CountDownActor = Cast<AMH_CountDownActor>(FoundActors[0]);
		if (CountDownActor)
		{
			// CountDownActor의 함수를 호출
			CountDownActor->MulticastStartCountDownVisible(true);
		}
	}
	MulticastShowOnlyNumPlayers();

	FTimerHandle RouletteTimerHandle;
	GetWorldTimerManager().SetTimer(RouletteTimerHandle , this , &ATTLuckyDrawGameState::StartPlayRoulette , StartPlayRouletteDelayTime ,
	                                false);
}

void ATTLuckyDrawGameState::StartPlayRoulette()
{
	MulticastStartLuckyDraw();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld() , AMH_CountDownActor::StaticClass() , FoundActors);
	if (FoundActors.Num() > 0)
	{
		// 첫 번째 찾은 액터를 AMH_CountDownActor로 캐스팅
		AMH_CountDownActor* CountDownActor = Cast<AMH_CountDownActor>(FoundActors[0]);
		if (CountDownActor)
		{
			// CountDownActor의 함수를 호출
			CountDownActor->MulticastStartCountDownVisible(false);
		}
	}

	ATTLuckyDrawGameMode* GameMode = GetWorld()->GetAuthGameMode<ATTLuckyDrawGameMode>();
	if (!GameMode) return;
	StartRounds(GameMode->Round);
}

void ATTLuckyDrawGameState::StartRounds(int32 InTotalRounds)
{
	// 새로운 라운드 시작 전 타이머 초기화
	GetWorldTimerManager().ClearTimer(RoundTimerHandle);
	GetWorldTimerManager().ClearTimer(LuckyDrawLoseTimerHandle);
	
	TotalRounds = InTotalRounds;
	CurrentRound = 0;
	StartNextRound(); // 첫 번째 라운드 시작
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

	// *** 추가
	// 라인 비활성화 - 새로운 라운드 시작 전에 수행
	if (ALuckyDrawLine* LineManager = Cast<ALuckyDrawLine>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ALuckyDrawLine::StaticClass())))
	{
		LineManager->DeactivateAllLines();
	}
	// *** 추가
	
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
			UGameplayStatics::GetActorOfClass(GetWorld(), ALuckyDrawManager::StaticClass()));
    	ALDTableManager* TableManager = Cast<ALDTableManager>(
			UGameplayStatics::GetActorOfClass(GetWorld(), ALDTableManager::StaticClass()));
        
    	// LuckyDrawLine 액터 찾기
    	ALuckyDrawLine* LineManager = Cast<ALuckyDrawLine>(
			UGameplayStatics::GetActorOfClass(GetWorld(), ALuckyDrawLine::StaticClass()));

    	if (!ChairManager || !TableManager || !LineManager) return;

    	// 현재 라운드의 룰렛 정보 가져오기
    	const FRouletteInfo& CurrentRoundInfo = GameMode->GetRouletteInfoForRound(LastRoundIndex);
        
    	// 선택된 플레이어의 위치 찾기
    	int32 SelectedRow = -1;
    	int32 SelectedCol = -1;
    	GameMode->GetPlayerPosition(CurrentRoundInfo.Player, SelectedRow, SelectedCol);

    	// 룰렛 규칙에 따라 라인 활성화
    	LineManager->ActivateLinesByRule(CurrentRoundInfo.Rule, SelectedRow, SelectedCol);

    	// 오버랩된 모든 의자 가져오기
    	TArray<ALuckyDrawChair*> AffectedChairs = LineManager->GetOverlappingChairs();

    	for (int32 PlayerID : GameMode->EliminatedPlayersPerRound[LastRoundIndex])
    	{
    		// 기존 탈락 플레이어 처리 로직...
    		for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
    		{
    			ATTPlayer* Player = *It;
    			if (Player && Player->GetRandomSeatNumber() == PlayerID)
    			{
    				Player->ClientLuckyDrawLose();
    				Player->MulticastSetColorTextRender(FColor(255, 0, 235));

    				// 테이블 색상 변경
    				FString* TargetChairTag = ChairManager->PlayerToChairMap.Find(PlayerID);
    				if (TargetChairTag)
    				{
    					FString TargetTableTag = TEXT("Table_") + TargetChairTag->RightChop(6);
    					TArray<UChildActorComponent*> TableComponents;
    					TableManager->GetComponents(TableComponents);

    					for (UChildActorComponent* TableComponent : TableComponents)
    					{
    						if (TableComponent && TableComponent->ComponentTags.Contains(FName(*TargetTableTag)))
    						{
    							if (ALuckyDrawTable* TargetTable = Cast<ALuckyDrawTable>(TableComponent->GetChildActor()))
    							{
    								TargetTable->SetColorRed();
    							}
    							break;
    						}
    					}
    				}
    			}
    		}
    	}

    	// 영향을 받은 모든 의자 날리기
    	for (ALuckyDrawChair* Chair : AffectedChairs)
    	{
    		UE_LOG(LogTemp, Log, TEXT("Throwing chair: %s"), *Chair->GetName());
    		Chair->ThrowChair();
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
	if (!GameMode) return;

	// 각 라운드에 맞는 룰렛 정보를 가져옴
	const FRouletteInfo& Info = GameMode->GetRouletteInfoForRound(CurrentRound);

	// 서버에서 클라이언트로 룰렛 정보 동기화
	MulticastUpdateRouletteUI(Info.Player , static_cast<int32>(Info.Rule) , static_cast<int32>(Info.Result));

	if (GameUI)
	{
		MulticastPlayRouletteAnimation();
	}

	CurrentRound++;
	// 다음 라운드 시작
	StartNextRound();
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

	// *** 추가
	// 라인 비활성화 - 새로운 라운드 시작 전에 수행
	if (ALuckyDrawLine* LineManager = Cast<ALuckyDrawLine>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ALuckyDrawLine::StaticClass())))
	{
		LineManager->DeactivateAllLines();
	}
	// *** 추가
	
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
			TTPlayer->MulticastSetColorTextRender(FColor(0 , 220 , 255));
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
}

void ATTLuckyDrawGameState::MulticastEndRounds_Implementation()
{
	if (GameUI)
	{
		GameUI->HideWidget();
	}
}
