// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HM_PuzzlePiece.generated.h"

UCLASS()
class MTVS3_FINAL_API AHM_PuzzlePiece : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHM_PuzzlePiece();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

    UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* Piece;

	// 스태틱 메시 컴포넌트 반환 함수 추가
	UStaticMeshComponent* GetPiece() const { return Piece; }

	// --------------- Multiplayer 요소들 ---------------
	
	// 물리 시뮬레이션 상태를 복제하기 위한 함수들
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    
	// 현재 Transform을 서버에서 클라이언트로 복제
	UPROPERTY(ReplicatedUsing = OnRep_PieceTransform)
	FTransform CurrentTransform;

	UFUNCTION()
	void OnRep_PieceTransform();

	// 물리 시뮬레이션 상태
	UPROPERTY(Replicated)
	bool bSimulatingPhysics;

	// // 클라이언트가 조각을 놓을 때 호출할 RPC 함수
	// UFUNCTION(Server, Reliable)
	// void Server_SetPieceLocation(const FVector& NewLocation);
	//
	// void PlacePiece(const FVector& NewLocation);
	// //void Server_SetPieceLocation_Implementation(const FVector& NewLocation);
};
