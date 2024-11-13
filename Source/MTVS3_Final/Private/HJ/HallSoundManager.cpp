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
}

// Called when the game starts or when spawned
void AHallSoundManager::BeginPlay()
{
	Super::BeginPlay();

	PlayConcertBGM();
}

// Called every frame
void AHallSoundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHallSoundManager::PlayConcertBGM()
{
	if (ConcertBGMCue && bPlayConcertBGM)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ConcertBGMCue, ConcertBGMLocation, 0.75f, 1.0f, 0.0f, ConcertAttenuation);
	}
}

void AHallSoundManager::PlayPlazaBGM()
{
	if (PlazaBGMCue)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PlazaBGMCue, PlazaBGMLocation, 0.75f, 1.0f, 0.0f, PlazaAttenuation);
	}
}

