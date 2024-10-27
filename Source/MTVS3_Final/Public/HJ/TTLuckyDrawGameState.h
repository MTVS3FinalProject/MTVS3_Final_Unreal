// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TTLuckyDrawGameState.generated.h"

/**
 *
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRequestMovePlayersToChairs);

UCLASS()
class MTVS3_FINAL_API ATTLuckyDrawGameState : public AGameState
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	// GameUI 초기화 상태를 추적하는 변수 추가
	bool bIsGameUIInitialized = false;
	
public:
#pragma region UI
	UPROPERTY(EditAnywhere, Category = "Defalut|UI")
	TSubclassOf<class UMH_GameWidget> GameUIFactory;
	UPROPERTY()
	class UMH_GameWidget* GameUI;
#pragma endregion
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRequestMovePlayersToChairs OnRequestMovePlayersToChairs;
	
	void AssignSeatNumber(APlayerState* PlayerState);
	void StartLuckyDraw();

	UFUNCTION()
	void MovePlayersToChairs();

	int32 CurrentSeatNumber = 1;

	UPROPERTY(ReplicatedUsing=OnRep_NewSeatNumber)
	int32 NewSeatNumber = -1;

	UFUNCTION()
	void OnRep_NewSeatNumber();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdatePlayerNumUI(int32 PlayerNum);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastStartLuckyDraw();

	void PlayRouletteAnimation();
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayRouletteAnimation();
};
