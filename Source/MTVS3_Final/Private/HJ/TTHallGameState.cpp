// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTHallGameState.h"
#include "EngineUtils.h"
#include "HJ/TTGameInstance.h"
#include "HJ/TTPlayer.h"
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
			TTPlayer->ServerSetTitleNameAndRarity(GI->GetTitleName(),GI->GetTitleRarity());
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
		if (TTPlayer && NicknameList.Contains(TTPlayer->GetNickname()))
		{
			if (TTPlayer->bIsHost) // 호스트에게는 추첨 시작 시간을 알림
			{
				if (TTPlayer->HasAuthority()) // 서버/호스트
				{
					// 서버에서 실행되는 로직
					ATTPlayerController* TTPC = Cast<ATTPlayerController>(TTPlayer->GetController());
					if (TTPC)
					{
						TTPC->SetDrawStartTime();
					}
					TTPlayer->ClientShowLuckyDrawInvitation(true, CompetitionRate);
					TTPlayer->SetLuckyDrawSeatID("1");
				}
				else // 클라이언트 호스트
				{
					TTPlayer->ClientShowLuckyDrawInvitation(true, CompetitionRate);
					TTPlayer->SetLuckyDrawSeatID("1");
                
					ATTPlayerController* TTPC = Cast<ATTPlayerController>(TTPlayer->GetController());
					if (TTPC)
					{
						TTPC->SetDrawStartTime();
					}
				}
			}
			else // 호스트가 아닐 때는 클라이언트 RPC로 초청
			{
				if (TTPlayer && !TTPlayer->bIsHost)
				{
					TTPlayer->ClientShowLuckyDrawInvitation(true , CompetitionRate);
					TTPlayer->SetLuckyDrawSeatID("1");
				}
			}
		}
	}
	
	// // 기존 타이머가 있다면 제거
	// if (GetWorldTimerManager().IsTimerActive(HideLDInvitationTimerHandle))
	// {
	// 	GetWorldTimerManager().ClearTimer(HideLDInvitationTimerHandle);
	// }
 //    
	// // NicknameList를 복사하여 캡처
	// TArray<FString> CapturedList = NicknameList;
    
	// GetWorldTimerManager().SetTimer(HideLDInvitationTimerHandle, 
	// 	[this, CapturedList, CompetitionRate]()
	// 	{
	// 		if (IsValid(this))
	// 		{
	// 			HideLuckyDrawInvitation(CapturedList, CompetitionRate);
	// 		}
	// 	}, 90.0f, false);
}

// void ATTHallGameState::HideLuckyDrawInvitation(const TArray<FString>& NicknameList , int32 CompetitionRate)
// {
// 	if (!IsValid(this))
// 	{
// 		return;
// 	}
//
// 	UWorld* World = GetWorld();
// 	if (!World || NicknameList.Num() == 0)
// 	{
// 		UE_LOG(LogTemp, Warning, TEXT("World is invalid or NicknameList is empty."));
// 		return;
// 	}
//
// 	for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
// 	{
// 		if (!*It)
// 		{
// 			continue;
// 		}
//     
// 		ATTPlayer* TTPlayer = *It;
// 		if (!IsValid(TTPlayer))
// 		{
// 			continue;
// 		}
//     
// 		// 추가 안전장치
// 		if (!TTPlayer->GetNickname().IsEmpty())
// 		{
// 			if (!TTPlayer->bIsHost && NicknameList.Contains(TTPlayer->GetNickname()))
// 			{
// 				TTPlayer->ClientShowLuckyDrawInvitation(false, CompetitionRate);
// 			}
// 		}
//
// 		if (!TTPlayer->bIsHost)
// 		{
// 			if (NicknameList.Contains(TTPlayer->GetNickname()))
// 			{
// 				TTPlayer->ClientShowLuckyDrawInvitation(false , CompetitionRate);
// 			}
// 		}
// 	}
// }
