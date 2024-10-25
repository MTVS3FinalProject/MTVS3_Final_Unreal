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
	class UBoxComponent* BoxComp;;

    UPROPERTY(EditDefaultsOnly)
	class UStaticMeshComponent* Piece;

	// 스태틱 메시 컴포넌트 반환 함수 추가
	UStaticMeshComponent* GetPiece() const { return Piece; }

};
