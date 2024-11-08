// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTHallGameState.h"
#include "EngineUtils.h"
#include "HJ/TTGameInstance.h"
#include "HJ/TTPlayer.h"
#include "HJ/TTPlayerState.h"
#include "JMH/MainWidget.h"
#include "JMH/MH_TicketingWidget.h"
#include "LHM/TTPlayerController.h"

void ATTHallGameState::BeginPlay()
{
	Super::BeginPlay();

	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if (GI) GI->SetPlaceState(EPlaceState::Plaza);

	for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
	{
		ATTPlayer* TTPlayer = *It;
		if (TTPlayer && TTPlayer->IsLocallyControlled())
		{
			TTPlayer->InitMainUI();
			TTPlayer->ServerSetNickname(GI->GetNickname());
			TTPlayer->SwitchCamera(TTPlayer->bIsThirdPerson);
		}
	}
}

void ATTHallGameState::SendLuckyDrawInvitation(const TArray<FString>& NicknameList , int32 CompetitionRate)
{
	if (!GetWorld() || NicknameList.Num() == 0)
	{
		UE_LOG(LogTemp , Warning , TEXT("GetWorld() is invalid or NicknameList is empty."));
		return;
	}

	for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
	{
		ATTPlayer* TTPlayer = *It;
		if (TTPlayer && TTPlayer->bIsHost)
		{
			if (TTPlayer->bIsHost) // 호스트에게는 추첨 시작 시간을 알림
			{
				ATTPlayerController* TTPC = Cast<ATTPlayerController>(TTPlayer->GetController());
				if (TTPC)
				{
					TTPC->SetDrawStartTime();
				}
			}
			else // 호스트가 아닐 때는 클라이언트 RPC로 초청
			{
				if (TTPlayer && !TTPlayer->bIsHost)
				{
					if (NicknameList.Contains(TTPlayer->GetNickname()))
					{
						TTPlayer->ClientShowLuckyDrawInvitation(true , CompetitionRate);
					}
				}
			}
		}
	}

	FTimerHandle HideLDInvitationTimerHandle;
	GetWorldTimerManager().SetTimer(HideLDInvitationTimerHandle , [this, NicknameList, CompetitionRate]()
	{
		HideLuckyDrawInvitation(NicknameList , CompetitionRate);
	} , 90.0f , false);
}

void ATTHallGameState::HideLuckyDrawInvitation(const TArray<FString>& NicknameList , int32 CompetitionRate)
{
	if (!GetWorld() || NicknameList.Num() == 0)
	{
		UE_LOG(LogTemp , Warning , TEXT("GetWorld() is invalid or NicknameList is empty."));
		return;
	}

	for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
	{
		ATTPlayer* TTPlayer = *It;

		if (!TTPlayer)
		{
			UE_LOG(LogTemp , Warning , TEXT("Found null TTPlayer"));
			continue;
		}

		if (!IsValid(TTPlayer))
		{
			UE_LOG(LogTemp , Warning , TEXT("Found invalid TTPlayer"));
			continue;
		}

		if (!TTPlayer->bIsHost)
		{
			if (NicknameList.Contains(TTPlayer->GetNickname()))
			{
				TTPlayer->ClientShowLuckyDrawInvitation(false , CompetitionRate);
			}
		}
	}
}
