// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTHallGameState.h"
#include "HJ/TTGameInstance.h"

void ATTHallGameState::BeginPlay()
{
	Super::BeginPlay();

	UTTGameInstance* GI = GetWorld()->GetGameInstance<UTTGameInstance>();
	if ( GI ) GI->SetPlaceState(EPlaceState::Plaza);
}