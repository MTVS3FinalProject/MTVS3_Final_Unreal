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
	if ( GI ) GI->SetPlaceState(EPlaceState::Plaza);

	for ( TActorIterator<ATTPlayer> It(GetWorld()); It; ++It )
	{
		ATTPlayer* TTPlayer = *It;
		if ( TTPlayer && TTPlayer->IsLocallyControlled() )
		{
			// ATTPlayerController* TTPC = TTPlayer->GetController<ATTPlayerController>();
			// TTPC->InitMainUI();
			// TTPlayer->ServerSetNickname(GI->GetNickname());
			TTPlayer->SwitchCamera(TTPlayer->bIsThirdPerson);
		}
	}
}
