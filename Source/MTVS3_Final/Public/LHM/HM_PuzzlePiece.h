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

    UPROPERTY(EditDefaultsOnly);
	TArray<UStaticMeshComponent*> Pieces;

	// 각 컴포넌트의 소유자를 저장하는 맵 추가
	UPROPERTY(EditDefaultsOnly);
	TMap<UStaticMeshComponent*, class AHM_PuzzlePlayer*> ComponentOwners;
	
	// 특정 컴포넌트의 소유자를 설정하는 함수
	void SetComponentOwner(UStaticMeshComponent* Component, class AHM_PuzzlePlayer* NewOwner);
	// 특정 컴포넌트의 소유자를 해제하는 함수
	void ClearComponentOwner(UStaticMeshComponent* Component);
	// 특정 컴포넌트가 소유 중인지 확인하는 함수
	bool IsComponentOwned(UStaticMeshComponent* Component);
	// 소유자를 반환하는 함수
	class AHM_PuzzlePlayer* GetComponentOwner(UStaticMeshComponent* Component);
	
	// 스태틱 메시 컴포넌트 반환 함수 추가
	const TArray<UStaticMeshComponent*>& GetAllPieces() const { return Pieces; }

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

	void InitializePieces();
};
