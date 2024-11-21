// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LuckyDrawTable.generated.h"

UCLASS()
class MTVS3_FINAL_API ALuckyDrawTable : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALuckyDrawTable();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY()
	FTransform InitialTableMeshTransform;
    
	UPROPERTY()
	FTransform InitialFloorMeshTransform;
    
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly)
	class USceneComponent* SceneComp;

	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* BoxComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* TableMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* FloorMeshComp;

	UPROPERTY(EditDefaultsOnly)
	class UTextRenderComponent* TextRenderComp;

	UFUNCTION(NetMulticast , Reliable)
	void MulticastSetTextRender(const FText& _RandomSeatNumber);

	UFUNCTION(BlueprintImplementableEvent , BlueprintCallable)
	void SetColorBlue();

	UFUNCTION(BlueprintImplementableEvent , BlueprintCallable)
	void SetColorRed();

	UFUNCTION(BlueprintImplementableEvent , BlueprintCallable)
	void SetColorBlack();

	UFUNCTION(NetMulticast , Reliable)
	void MulticastSetPhysicsState(bool bSimulate);

	UFUNCTION(NetMulticast , Reliable)
	void MulticastResetTable();

private:
	FVector OriginalLocation;
	FRotator OriginalRotation;
};
