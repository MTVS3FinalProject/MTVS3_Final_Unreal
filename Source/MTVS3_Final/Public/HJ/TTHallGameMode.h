// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TTHallGameMode.generated.h"

/**
 * 
 */
UCLASS()
class ATTHallGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	// 현재 사용할 PlayerStart의 인덱스를 저장
	UPROPERTY()
	int32 CurrentPlayerStartIndex = 0;

	// PlayerStart 배열 캐시
	UPROPERTY()
	TArray<AActor*> CachedPlayerStarts;

	// 게임 시작시 PlayerStart 캐시
	virtual void BeginPlay() override;
    
	// PlayerStart 선택 로직 오버라이드
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};
