// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TTHallGameState.generated.h"

/**
 *
 */
UCLASS()
class MTVS3_FINAL_API ATTHallGameState : public AGameState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	void SendLuckyDrawInvitation(const TArray<FString>& NicknameList, int32 CompetitionRate);

	void HideLuckyDrawInvitation(const TArray<FString>& NicknameList, int32 CompetitionRate);
};
