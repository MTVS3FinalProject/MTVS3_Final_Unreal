// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/HallSoundManager.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundCue.h"

// Sets default values
AHallSoundManager::AHallSoundManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USoundCue> tempConcertBGMCue(TEXT("/Script/Engine.SoundCue'/Game/KHJ/Assets/Sounds/A_CONCERT_BGM_ROUGH_Cue.A_CONCERT_BGM_ROUGH_Cue'"));
	if (tempConcertBGMCue.Succeeded())
	{
		ConcertBGMCue = tempConcertBGMCue.Object;
		ConcertBGMCue->Priority = 0.5f; // 우선 순위 설정
		ConstructorHelpers::FObjectFinder<USoundAttenuation> tempConcertAttenuation(TEXT("/Script/Engine.SoundAttenuation'/Game/KHJ/Assets/Sounds/ATT_ConcertAttenuation.ATT_ConcertAttenuation'"));
		if (tempConcertAttenuation.Succeeded())
		{
			ConcertAttenuation = tempConcertAttenuation.Object;
		}
	}

	ConstructorHelpers::FObjectFinder<USoundCue> tempPlazaBGMCue(TEXT("/Script/Engine.SoundCue'/Game/KHJ/Assets/Sounds/A_LOBBY_BGM_Cue.A_LOBBY_BGM_Cue'"));
	if (tempPlazaBGMCue.Succeeded())
	{
		PlazaBGMCue = tempPlazaBGMCue.Object;
		PlazaBGMCue->Priority = 0.5f; // 우선 순위 설정
		ConstructorHelpers::FObjectFinder<USoundAttenuation> tempPlazaAttenuation(TEXT("/Script/Engine.SoundAttenuation'/Game/KHJ/Assets/Sounds/ATT_PlazaAttenuation.ATT_PlazaAttenuation'"));
		if (tempPlazaAttenuation.Succeeded())
		{
			PlazaAttenuation = tempPlazaAttenuation.Object;
		}
	}
}

// Called when the game starts or when spawned
void AHallSoundManager::BeginPlay()
{
	Super::BeginPlay();

	PlayPlazaBGM();
	// PlayConcertBGM();
}

// Called every frame
void AHallSoundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHallSoundManager::SetbPlayConcertBGM(bool _bPlayConcertBGM)
{
	bPlayConcertBGM = _bPlayConcertBGM;
}

void AHallSoundManager::PlayConcertBGM()
{
	APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (PC && PC->IsLocalController())
	{
		if (ConcertBGMCue && bPlayConcertBGM)
		{
			UGameplayStatics::PlaySoundAtLocation(this, ConcertBGMCue, ConcertBGMLocation, 0.75f, 1.0f, 0.0f, ConcertAttenuation);
		}
	}
}

void AHallSoundManager::PlayPlazaBGM()
{
	APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (PC && PC->IsLocalController())
	{
		if (PlazaBGMCue)
		{
			UGameplayStatics::PlaySoundAtLocation(this, PlazaBGMCue, PlazaBGMLocation, 0.75f, 1.0f, 0.0f, PlazaAttenuation);
		}
	}
}

