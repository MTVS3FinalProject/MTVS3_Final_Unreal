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
}

void AHM_PuzzleTriggerVolume::BeginPlay()
{
    Super::BeginPlay();
}

void AHM_PuzzleTriggerVolume::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent , AActor* OtherActor , UPrimitiveComponent* OtherComp , int32 OtherBodyIndex , bool bFromSweep , const FHitResult& SweepResult)
{
    AHM_PuzzlePlayer* Player = Cast<AHM_PuzzlePlayer>(OtherActor);
    if ( Player && Player->HasPickedUpPiece() )
    {
        // 플레이어가 블럭을 가지고 구역에 들어왔을 때 퍼즐 조각을 날림
        Player->LaunchPickedUpPiece();
    }
    GEngine->AddOnScreenDebugMessage(-1 , 3.f , FColor::Green , FString::Printf(TEXT("TriggerBox::OnOverlapBegin")));
}
