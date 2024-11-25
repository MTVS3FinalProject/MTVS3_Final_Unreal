// Fill out your copyright notice in the Description page of Project Settings.


#include "HJ/TTHallGameMode.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

void ATTHallGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!bEnableScreenDebug)
	{
#if UE_BUILD_DEVELOPMENT
		GEngine->bEnableOnScreenDebugMessages = false; // 화면 디버그 메시지 비활성화
#endif
	}
    
	// 모든 PlayerStart를 찾아서 캐시
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), CachedPlayerStarts);
}

AActor* ATTHallGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if(CachedPlayerStarts.Num() == 0)
	{
		// PlayerStart가 없으면 기본 동작 사용
		return Super::ChoosePlayerStart_Implementation(Player);
	}

	// 현재 인덱스의 PlayerStart 선택
	AActor* ChosenStart = CachedPlayerStarts[CurrentPlayerStartIndex];

	// 다음 인덱스 준비 (순환)
	CurrentPlayerStartIndex = (CurrentPlayerStartIndex + 1) % CachedPlayerStarts.Num();

	return ChosenStart;
}
