// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "HM_PuzzleTriggerVolume.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class MTVS3_FINAL_API AHM_PuzzleTriggerVolume : public ATriggerVolume
{
	GENERATED_BODY()

public:
    AHM_PuzzleTriggerVolume();

    UPROPERTY(EditDefaultsOnly)
    class UBoxComponent* TriggerBox;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, 
                       AActor* OtherActor, 
                       UPrimitiveComponent* OtherComp, 
                       int32 OtherBodyIndex, 
                       bool bFromSweep, 
                       const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex);
};
