// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTHallGameState.h"
#include "EngineUtils.h"
#include "HJ/TTGameInstance.h"
#include "HJ/TTPlayer.h"
#include "LHM/TTPlayerController.h"
#include "Net/UnrealNetwork.h"

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

void ATTHallGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATTHallGameState, ReservedSeatIds);
	DOREPLIFETIME(ATTHallGameState, LuckyDrawSeatId);
}

void ATTHallGameState::SendLuckyDrawInvitation(const TArray<FString>& NicknameList, const FString& SeatInfo, int32 CompetitionRate)
{
	UE_LOG(LogTemp, Log, TEXT("좌석 번호 %s에 대한 추첨 초대 시작. 대상자 수: %d"), *LuckyDrawSeatId, NicknameList.Num());
	if (!GetWorld() || NicknameList.Num() == 0)
	{
		UE_LOG(LogTemp , Warning , TEXT("GetWorld() is invalid or NicknameList is empty."));
		return;
	}

	for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
	{
		ATTPlayer* TTPlayer = *It;
		if (!IsValid(TTPlayer)) continue;
		
		if (TTPlayer && NicknameList.Contains(TTPlayer->GetNickname()))
		{
			TTPlayer->ClientShowLuckyDrawInvitation(true, SeatInfo, CompetitionRate);
			// TTPlayer->SetLuckyDrawSeatID("1");
			TTPlayer->SetLuckyDrawSeatID(LuckyDrawSeatId);
		}
		else if (TTPlayer && TTPlayer->bIsHost)
		{
			TTPlayer->ClientShowLuckyDrawInvitation(true, SeatInfo, CompetitionRate);
			TTPlayer->SetLuckyDrawSeatID(LuckyDrawSeatId);
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

void ATTHallGameState::SetLuckyDrawSeatId(const FString& NewSeatId)
{
	LuckyDrawSeatId = NewSeatId;
}

void ATTHallGameState::MulticastUpdateChairStates_Implementation(const TArray<int32>& ReservedSeats)
{
	ReservedSeatIds = ReservedSeats;  // 기존 TArray는 유지 
	ReservedSeatIdsSet = TSet<int32>(ReservedSeats);  // TSet으로 변환
	OnChairStatesUpdated.Broadcast();
}

void ATTHallGameState::Multicast_ApplyTicketImage_Implementation(int32 TicketIndex, const FString& TicketImgUrl)
{
	OnTicketImageUpdated.Broadcast(TicketIndex, TicketImgUrl);
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

