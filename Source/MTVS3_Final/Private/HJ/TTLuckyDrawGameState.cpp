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
        MulticastStartLuckyDraw();
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
    
    FTimerHandle RouletteTimerHandle;
    GetWorldTimerManager().SetTimer(RouletteTimerHandle, this, &ATTLuckyDrawGameState::PlayRouletteAnimation, 6.0f, false);
}

void ATTLuckyDrawGameState::PlayRouletteAnimation()
{
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
    
    if (GameUI)
    {
        GameUI->PlayRouletteAnim();
        MulticastPlayRouletteAnimation();
    }
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
    GameUI->SetWidgetSwitcher(1);
}
