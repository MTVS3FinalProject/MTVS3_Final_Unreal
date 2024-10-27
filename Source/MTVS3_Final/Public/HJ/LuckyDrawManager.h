// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TTPlayer.h"
#include "GameFramework/Actor.h"
#include "LuckyDrawManager.generated.h"

UCLASS()
class MTVS3_FINAL_API ALuckyDrawManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALuckyDrawManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ConvertSeatAssignments();
	void MovePlayersToChairs();
	ATTPlayer* GetPlayerBySeatNumber(int32 SeatNumber);

	TMap<int32, FString> PlayerToChairMap;
};
