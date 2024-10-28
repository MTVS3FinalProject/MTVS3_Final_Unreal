#include "HJ/TTLuckyDrawGameState.h"
#include "HJ/TTGameInstance.h"
//#include <HJ/TTPlayerState.h>
//#include "LHM/TTPlayerController.h"
#include <HJ/TTPlayer.h>
#include "EngineUtils.h"
#include "HJ/LuckyDrawManager.h"
#include "HJ/TTLuckyDrawGameMode.h"
#include "JMH/MH_CountDownActor.h"
#include "JMH/MH_GameWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ATTLuckyDrawGameState::BeginPlay()
{
	Super::BeginPlay();

	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if ( GI ) GI->SetPlaceState(EPlaceState::LuckyDrawRoom);

	CurrentSeatNumber = 0;

    GameUI = CastChecked<UMH_GameWidget>(CreateWidget(GetWorld() , GameUIFactory));
    if ( GameUI )
    {
        GameUI->AddToViewport();
        GameUI->SetWidgetSwitcher(0);
    }

    for ( TActorIterator<ATTPlayer> It(GetWorld()); It; ++It )
    {
        ATTPlayer* TTPlayer = *It;
        if ( TTPlayer && !TTPlayer->GetbIsHost() && TTPlayer->IsLocallyControlled() )
        {
            TTPlayer->InitGameUI();
        }
    }
}

void ATTLuckyDrawGameState::AssignSeatNumber(APlayerState* PlayerState)
{
    ATTLuckyDrawGameMode* GameMode = GetWorld()->GetAuthGameMode<ATTLuckyDrawGameMode>();
    if (!GameMode) return;
    for ( TActorIterator<ATTPlayer> It(GetWorld()); It; ++It )
    {
        ATTPlayer* TTPlayer = *It;
        if ( TTPlayer && !TTPlayer->GetbIsHost() && TTPlayer->GetRandomSeatNumber() == -1 )
        {
            TTPlayer->SetRandomSeatNumber(++CurrentSeatNumber);

            // 디버그 메시지
            GEngine->AddOnScreenDebugMessage(-1 , 5.f , FColor::Green ,
                FString::Printf(TEXT("플레이어 %s에게 좌석 번호 %d가 할당되었습니다.") ,
                    *TTPlayer->GetNickname() , TTPlayer->GetRandomSeatNumber()));

            NewSeatNumber = TTPlayer->GetRandomSeatNumber();
            
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
    
    ALuckyDrawManager* Manager = Cast<ALuckyDrawManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ALuckyDrawManager::StaticClass()));
    if (Manager)
    {
        Manager->ConvertSeatAssignments();
        UE_LOG(LogTemp, Log, TEXT("Lucky Draw Started: Players moved to their designated chairs."));
    }

    for ( TActorIterator<ATTPlayer> It(GetWorld()); It; ++It )
    {
        ATTPlayer* TTPlayer = *It;
        if ( TTPlayer && !TTPlayer->GetbIsHost())
        {
            TTPlayer->MulticastLuckyDrawStart();
        }
    }

    // 카운트 다운 시작
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMH_CountDownActor::StaticClass(), FoundActors);
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
    MulticastHideGameUI();
    
    FTimerHandle RouletteTimerHandle;
    GetWorldTimerManager().SetTimer(RouletteTimerHandle, this, &ATTLuckyDrawGameState::StartPlayRoulette, 6.0f, false);
}

void ATTLuckyDrawGameState::StartPlayRoulette()
{
    MulticastStartLuckyDraw();
    
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMH_CountDownActor::StaticClass(), FoundActors);
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

void ATTLuckyDrawGameState::MulticastUpdatePlayerNumUI_Implementation(int32 PlayerNum)
{
    PlayerNum = NewSeatNumber;
}

void ATTLuckyDrawGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(ATTLuckyDrawGameState, NewSeatNumber);
}

void ATTLuckyDrawGameState::MulticastStartLuckyDraw_Implementation()
{
    GameUI->ShowWidget();
    GameUI->SetWidgetSwitcher(1);
}

void ATTLuckyDrawGameState::StartRounds(int32 InTotalRounds)
{
    TotalRounds = InTotalRounds;
    CurrentRound = 0;
    StartNextRound();  // 첫 번째 라운드 시작
}

void ATTLuckyDrawGameState::StartNextRound()
{
    ATTLuckyDrawGameMode* GameMode = GetWorld()->GetAuthGameMode<ATTLuckyDrawGameMode>();
    if (!GameMode) return;
    if (CurrentRound > 0 && GameMode->EliminatedPlayersPerRound.Num() >= CurrentRound)
    {
        int32 LastRoundIndex = CurrentRound - 1;
        for (int32 PlayerID : GameMode->EliminatedPlayersPerRound[LastRoundIndex])
        {
            for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
            {
                ATTPlayer* Player = *It;
                if (Player && Player->GetRandomSeatNumber() == PlayerID)
                {
                    Player->ClientLuckyDrawLose();
                }
            }
        }
    }
    
    if (CurrentRound >= TotalRounds)
    {
        FTimerHandle RouletteTimerHandle;
        GetWorldTimerManager().SetTimer(RouletteTimerHandle, this, &ATTLuckyDrawGameState::EndRounds, 10.0f, false);
        return;
    }

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
    float Delay = (CurrentRound == 0) ? 0.5f : 10.0f;
    GetWorldTimerManager().SetTimer(RoundTimerHandle, this, &ATTLuckyDrawGameState::PlayRoulette, Delay, false);
}

void ATTLuckyDrawGameState::PlayRoulette()
{
    ATTLuckyDrawGameMode* GameMode = GetWorld()->GetAuthGameMode<ATTLuckyDrawGameMode>();
    if (!GameMode) return;

    // 각 라운드에 맞는 룰렛 정보를 가져옴
    const FRouletteInfo& Info = GameMode->GetRouletteInfoForRound(CurrentRound);

    // 서버에서 클라이언트로 룰렛 정보 동기화
    MulticastUpdateRouletteUI(Info.Player, static_cast<int32>(Info.Rule), static_cast<int32>(Info.Result));
    
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

void ATTLuckyDrawGameState::MulticastUpdateRouletteUI_Implementation(int32 Player, int32 Rule, int32 Result)
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
        if (TTPlayer && TTPlayer->GetRandomSeatNumber() == WinningSeatNumber)
        {
            TTPlayer->ClientLuckyDrawWin();  // 우승자에게만 종료 호출
            break;
        }
    }
}

void ATTLuckyDrawGameState::MulticastEndRounds_Implementation()
{
    if (GameUI)
    {
        GameUI->HideWidget();
    }
}