// Fill out your copyright notice in the Description page of Project Settings.


#include "LHM/HM_PuzzleTriggerVolume.h"
#include "LHM/HM_PuzzlePlayer.h"
#include "Components/BoxComponent.h"

AHM_PuzzleTriggerVolume::AHM_PuzzleTriggerVolume()
{
    TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
    RootComponent = TriggerBox;

    // 콜리전 설정
    TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    TriggerBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
    TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    TriggerBox->SetCollisionResponseToChannel(ECC_Pawn , ECR_Overlap);

    // 오버랩 이벤트 바인딩
    TriggerBox->OnComponentBeginOverlap.AddDynamic(this , &AHM_PuzzleTriggerVolume::OnOverlapBegin);
    TriggerBox->OnComponentEndOverlap.AddDynamic(this , &AHM_PuzzleTriggerVolume::OnOverlapEnd);
}

void AHM_PuzzleTriggerVolume::BeginPlay()
{
    Super::BeginPlay();
}

void AHM_PuzzleTriggerVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent ,
    AActor* OtherActor ,
    UPrimitiveComponent* OtherComp ,
    int32 OtherBodyIndex ,
    bool bFromSweep ,
    const FHitResult& SweepResult)
{
    AHM_PuzzlePlayer* Player = Cast<AHM_PuzzlePlayer>(OtherActor);
    if ( Player )
    {
        Player->SetIsInTriggerZone(true);
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, 
            FString::Printf(TEXT("Entered Launch Zone")));
    }
}

void AHM_PuzzleTriggerVolume::OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    AHM_PuzzlePlayer* Player = Cast<AHM_PuzzlePlayer>(OtherActor);
    if (Player)
    {
        Player->SetIsInTriggerZone(false);
        GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, 
            FString::Printf(TEXT("Left Launch Zone")));
    }
}
