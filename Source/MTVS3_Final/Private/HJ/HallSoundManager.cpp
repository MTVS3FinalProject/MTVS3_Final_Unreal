// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/HallSoundManager.h"
#include "Components/AudioComponent.h"
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
	UpdateConcertBGMVolume(bPlayConcertBGM);
}

void AHallSoundManager::PlayConcertBGM()
{
	APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (PC && PC->IsLocalController())
	{
		// 이미 존재하는 AudioComponent가 있다면 중지
		if (ConcertBGMAudioComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("Stopping existing Concert BGM"));
			ConcertBGMAudioComponent->Stop();
			ConcertBGMAudioComponent = nullptr;
		}

		if (ConcertBGMCue)
		{
			UE_LOG(LogTemp, Warning, TEXT("Creating new Concert BGM"));
			ConcertBGMAudioComponent = UGameplayStatics::SpawnSoundAtLocation(
				this, 
				ConcertBGMCue, 
				ConcertBGMLocation, 
				FRotator::ZeroRotator,
				bPlayConcertBGM ? 0.75f : 0.0f, // 초기 볼륨 설정
				1.0f,
				0.0f,
				ConcertAttenuation
			);

			// AudioComponent가 생성되었다면 루프 설정
			if (ConcertBGMAudioComponent)
			{
				ConcertBGMAudioComponent->bAutoDestroy = false;  // 자동 파괴 방지
			}
		}
	}
}

void AHallSoundManager::UpdateConcertBGMVolume(bool _bPlayConcertBGM)
{
	if (ConcertBGMAudioComponent)
	{
		float targetVolume = _bPlayConcertBGM ? 0.75f : 0.0f;
		ConcertBGMAudioComponent->SetVolumeMultiplier(targetVolume);
        
		// 재생이 멈췄다면 다시 재생
		if (!ConcertBGMAudioComponent->IsPlaying())
		{
			UE_LOG(LogTemp, Warning, TEXT("Restarting Concert BGM"));
			ConcertBGMAudioComponent->Play();
		}
        
		UE_LOG(LogTemp, Warning, TEXT("Concert BGM Status - Volume: %f, IsPlaying: %d, IsValid: %d"), 
			ConcertBGMAudioComponent->VolumeMultiplier,
			ConcertBGMAudioComponent->IsPlaying(),
			IsValid(ConcertBGMAudioComponent));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ConcertBGMAudioComponent is null!"));
		// AudioComponent가 없다면 새로 생성
		PlayConcertBGM();
	}
}

void AHallSoundManager::PlayPlazaBGM()
{
	APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (PC && PC->IsLocalController())
	{
		// 이미 존재하는 AudioComponent가 있다면 중지
		if (PlazaBGMAudioComponent)
		{
			UE_LOG(LogTemp, Warning, TEXT("Stopping existing Plaza BGM"));
			PlazaBGMAudioComponent->Stop();
			PlazaBGMAudioComponent = nullptr;
		}

		if (PlazaBGMCue)
		{
			UE_LOG(LogTemp, Warning, TEXT("Creating new Plaza BGM"));
			PlazaBGMAudioComponent = UGameplayStatics::SpawnSoundAtLocation(
				this,
				PlazaBGMCue,
				PlazaBGMLocation,
				FRotator::ZeroRotator,
				0.75f,
				1.0f,
				0.0f,
				PlazaAttenuation
			);
            
			if (PlazaBGMAudioComponent)
			{
				PlazaBGMAudioComponent->bAutoDestroy = false;
			}
		}
	}
}

