// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/LuckyDrawSoundManager.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundAttenuation.h"

// Sets default values
ALuckyDrawSoundManager::ALuckyDrawSoundManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USoundCue> tempLuckyDrawBGMCue(TEXT("/Script/Engine.SoundCue'/Game/KHJ/Assets/Sounds/A_LuckyDraw_BGM_Cue.A_LuckyDraw_BGM_Cue'"));
	if (tempLuckyDrawBGMCue.Succeeded())
	{
		LuckyDrawBGMCue = tempLuckyDrawBGMCue.Object;
		LuckyDrawBGMCue->Priority = 0.5f; // 우선 순위 설정
	}
}

// Called when the game starts or when spawned
void ALuckyDrawSoundManager::BeginPlay()
{
	Super::BeginPlay();
	PlayLuckyDrawBGM();
}

// Called every frame
void ALuckyDrawSoundManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALuckyDrawSoundManager::PlayLuckyDrawBGM()
{
	APlayerController* PC = GEngine->GetFirstLocalPlayerController(GetWorld());
	if (PC && PC->IsLocalController())
	{
		if (LuckyDrawBGMCue)
		{
			// 이미 재생 중인 BGM이 있다면 정지
			if (BGMAudioComponent)
			{
				BGMAudioComponent->Stop();
				BGMAudioComponent = nullptr;
			}

			// 새로운 BGM 재생 및 컴포넌트 저장
			BGMAudioComponent = UGameplayStatics::SpawnSound2D(
				this,           // WorldContextObject
				LuckyDrawBGMCue,    // Sound
				0.75f,         // Volume Multiplier
				1.0f,          // Pitch Multiplier
				0.0f,          // Start Time
				nullptr,       // Concurrency Settings
				true,          // bPersistAcrossLevelTransition
				false          // bAutoDestroy
			);
		}
	}
}

void ALuckyDrawSoundManager::StopLuckyDrawBGM()
{
	if (BGMAudioComponent)
	{
		BGMAudioComponent->Stop();
		BGMAudioComponent = nullptr;
	}
}

void ALuckyDrawSoundManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
    
	if (BGMAudioComponent)
	{
		BGMAudioComponent->Stop();
		BGMAudioComponent = nullptr;
	}
}

