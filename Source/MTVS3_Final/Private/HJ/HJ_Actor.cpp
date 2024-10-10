// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/HJ_Actor.h"
#include "HJ/TTPlayer.h"
#include "HJ/TTPlayerState.h"

// Sets default values
AHJ_Actor::AHJ_Actor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHJ_Actor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHJ_Actor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ATTPlayer* TTPlayer = Cast<ATTPlayer>(GetWorld()->GetFirstPlayerController()->GetPawn());
	if ( TTPlayer )
	{
		if ( ATTPlayerState* PS = TTPlayer->GetPlayerState<ATTPlayerState>())
		{
			PS->SetbIsHost(true);
			bool bIsHost = PS->GetbIsHost();
			PS->SetNickname("Nickname");
			FString Nickname = PS->GetNickname();
			PS->SetUserId(12345);
			int32 UserId = PS->GetUserId();
			PS->AddCoin(-30);
			int32 Coin = PS->GetCoin();
		}
	}
}

