// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HM_PuzzleBoard.generated.h"

UCLASS()
class MTVS3_FINAL_API AHM_PuzzleBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHM_PuzzleBoard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Puzzle Board")
	TArray<UStaticMeshComponent*> BoardAreas;

	UPROPERTY(EditAnywhere, Category = "Puzzle Board Settings")
	float CellSize = 700.0f;

	void InitializeBoardAreas();
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
// ==================================== 멀티 ====================================
	
	// 서버에서 호출되는 RPC 함수
	UFUNCTION(Server, Reliable)
	void ServerSetBoardAreaVisibility(int32 BoardIndex, bool bVisible);

	// 모든 클라이언트에서 호출되는 RPC 함수
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetBoardAreaVisibility(int32 BoardIndex, bool bVisible);
};
