// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/LuckyDrawManager.h"

#include "EngineUtils.h"
#include "Components/ChildActorComponent.h"
#include "HJ/LuckyDrawChair.h"
#include "HJ/TTLuckyDrawGameState.h"
#include "HJ/TTPlayer.h"

// Sets default values
ALuckyDrawManager::ALuckyDrawManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALuckyDrawManager::BeginPlay()
{
	Super::BeginPlay();

	// LuckyDrawChair 컴포넌트들을 배열에 저장
	TArray<UChildActorComponent*> LuckyDrawChairComponents;
	GetComponents(LuckyDrawChairComponents);

	// 인덱스 기반으로 각 컴포넌트에 태그 추가
	for (int32 Index = 0; Index < LuckyDrawChairComponents.Num(); ++Index)
	{
		UChildActorComponent* ChildComponent = LuckyDrawChairComponents[Index];
		if (ChildComponent && ChildComponent->GetChildActor())
		{
			// 태그를 "Chair_1", "Chair_2", ..., "Chair_30" 형식으로 추가
			FString Tag = FString::Printf(TEXT("Chair_%d"), Index + 1);
			ChildComponent->ComponentTags.Add(FName(*Tag));

			UE_LOG(LogTemp, Log, TEXT("Added Tag: %s to Chair Component at index %d"), *Tag, Index + 1);
		}
	}
}

// Called every frame
void ALuckyDrawManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALuckyDrawManager::MovePlayersToChairs()
{
	ATTLuckyDrawGameState* GameState = GetWorld()->GetGameState<ATTLuckyDrawGameState>();
	if (!GameState) return;

	// 각 플레이어를 순회하며 의자 위치로 이동
	for (TActorIterator<ATTPlayer> It(GetWorld()); It; ++It)
	{
		ATTPlayer* Player = *It;
		if (!Player) continue;

		if (Player->GetbIsHost() != true)
		{
			int32 SeatNumber = Player->GetRandomSeatNumber();
			FString TargetTag = FString::Printf(TEXT("Chair_%d"), SeatNumber);

			TArray<UChildActorComponent*> ChairComponents;
			GetComponents(ChairComponents);

			for (UChildActorComponent* ChairComponent : ChairComponents)
			{
				if (ChairComponent->ComponentTags.Contains(FName(*TargetTag)))
				{
					ALuckyDrawChair* TargetChair = Cast<ALuckyDrawChair>(ChairComponent->GetChildActor());
					if (TargetChair)
					{
						FTransform TargetTransform = TargetChair->GetSittingTransform();
						Player->SetActorTransform(TargetTransform);
						UE_LOG(LogTemp, Log, TEXT("Player %s moved to %s"), *Player->GetNickname(), *TargetTag);
					}
					break;
				}
			}
		}
	}
}

