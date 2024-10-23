// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTLuckyDrawGameState.h"
#include "HJ/TTGameInstance.h"

void ATTLuckyDrawGameState::BeginPlay()
{
	Super::BeginPlay();

	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if ( GI ) GI->SetPlaceState(EPlaceState::LuckyDrawRoom);
}
