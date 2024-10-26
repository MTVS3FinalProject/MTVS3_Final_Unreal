// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TTLuckyDrawGameState.generated.h"

/**
 *
 */
UCLASS()
class MTVS3_FINAL_API ATTLuckyDrawGameState : public AGameState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	void AssignSeatNumber(APlayerState* PlayerState);
	void StartLuckyDraw();

	int32 CurrentSeatNumber = 1;
};
