// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTPlayerState.h"

void ATTPlayerState::BeginPlay()
{
	Super::BeginPlay();

	//UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	//if ( GI ) SetNickname(GI->GetNickname());
}

void ATTPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(ATTPlayerState , Nickname);
}

//void ATTPlayerState::SetNickname(const FString& _Nickname)
//{
//	Nickname = _Nickname;
//
//	ServerSetNickname();
//}

//void ATTPlayerState::ServerSetNickname_Implementation()
//{
//	OnRep_Nickname();
//}

//void ATTPlayerState::OnRep_Nickname()
//{
//	ATTPlayerController* TTPC = Cast<ATTPlayerController>(GetOwner());
//	if ( !TTPC || !TTPC->GetPawn() )
//	{
//		// PlayerController나 Pawn이 없으면 0.1초 후 다시 시도
//		GetWorld()->GetTimerManager().SetTimer(
//			TimerHandle_OnRepRetry ,   // Timer handle
//			this ,                     // 호출할 객체
//			&ATTPlayerState::OnRep_Nickname ,  // 호출할 함수
//			1.0f ,                     // 0.1초 후에 호출
//			false                     // 반복하지 않음
//		);
//		return;
//	}
//
//	APawn* PlayerPawn = TTPC->GetPawn();
//	ATTPlayer* TTPlayer = Cast<ATTPlayer>(PlayerPawn);
//
//	if ( !TTPlayer || !TTPlayer->NicknameUI )
//	{
//		// NicknameUI가 없으면 0.1초 후 다시 시도
//		GetWorld()->GetTimerManager().SetTimer(
//			TimerHandle_OnRepRetry ,
//			this ,
//			&ATTPlayerState::OnRep_Nickname ,
//			1.0f ,
//			false
//		);
//		return;
//	}
//
//	TTPlayer->NicknameUI->UpdateNicknameUI(GetNickname());
//}

//void ATTPlayerState::SetLuckyDrawSeatID(const FString& _LuckyDrawSeatID)
//{
//	LuckyDrawSeatID = _LuckyDrawSeatID;
//
//	// GI에도 SetLuckyDrawSeatID
//	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
//	if ( GI ) GI->SetLuckyDrawSeatID(LuckyDrawSeatID);
//}

//void ATTPlayerState::SetRandomSeatNumber(const int32& _RandomSeatNumber)
//{
//	RandomSeatNumber = _RandomSeatNumber;
//}
