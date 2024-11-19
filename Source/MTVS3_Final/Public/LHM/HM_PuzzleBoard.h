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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UStaticMeshComponent*> BoardAreasGrid;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UStaticMeshComponent*> BoardAreas;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CellSize = 500.0f;

	void InitializeBoardAreas();
	
	void InitializeBoardAreasLocation();
	void InitializeBoardAreasGridLocation();
	
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	bool AreAllBoardAreasVisible() const;
	
// ==================================== 멀티 ====================================

	UPROPERTY(ReplicatedUsing = OnRep_DestroyedPieceTags)
	TArray<FString> DestroyedPieceTags;
	
	// 서버에서 호출되는 RPC 함수
	UFUNCTION(Server, Reliable)
	void ServerSetBoardAreaVisibility(int32 BoardIndex, bool bVisible);

	// 모든 클라이언트에서 호출되는 RPC 함수
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetBoardAreaVisibility(int32 BoardIndex, bool bVisible);

	// KHJ
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing=OnRep_BoardAreaVisibility)
	TArray<bool> BoardAreaVisibility;
	
	UFUNCTION()
	void OnRep_BoardAreaVisibility();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastDestroyPuzzlePiece(AHM_PuzzlePiece* PuzzlePiece, const FString& TagToDestroy);

	UFUNCTION()
	void OnRep_DestroyedPieceTags();
};
